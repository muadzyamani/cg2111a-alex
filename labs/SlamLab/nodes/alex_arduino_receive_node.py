# This node is an example of a simple publisher that receives messages from the arduino and publish it on the ardunio/recv topic
# Currently does not do anything else with the messages.

# Import Python Native Modules. We require the Barrier class from threading to synchronize the start of multiple threads.
import ctypes
from threading import Barrier

# Import the required pubsub modules. PubSubMsg class to extract the payload from a message.

from pubsub.pub_sub_manager import ManagedPubSubRunnable, PubSubMsg
from pubsub.pub_sub_manager import publish, subscribe, unsubscribe, getMessages, getCurrentExecutionContext  

# Import the required arduino communication modules. Replace or add to the handlers as needed.
from control.alex_control import receivePacket
from control.alex_control_constants import  TPacket, TPacketType, PAYLOAD_PARAMS_COUNT, PAYLOAD_PACKET_SIZE
from control.alex_control_constants import  TResponseType, TResultType, COMMS_BUFFER_SIZE, COMMS_MAGIC_NUMBER, COMMS_PACKET_SIZE, PAYLOAD_DATA_MAX_STR_LEN, TComms


# Constants
PUBLISH_PACKETS = True
ARDUINO_RECV_TOPIC = "arduino/recv" 



def receiveThread(setupBarrier:Barrier=None, readyBarrier:Barrier=None):
    """
    Thread function to handle receiving arduino packets in a loop until the context signals an exit.
    Args:
        setupBarrier (Barrier, optional): A threading barrier to synchronize the start of the thread setup.
        readyBarrier (Barrier, optional): A threading barrier to synchronize the thread start.
                                          If provided, the thread will wait for all parties to be ready before proceeding.
    The function performs the following steps:
    1. Sets up the execution context.
    2. Waits for all threads to be ready if barriers are provided.
    3. Enters a loop to receive arduino packets until the context signals an exit.
    4. Processes packets based on their type:
        - Handles response packets.
        - Handles error response packets.
        - Handles message packets.
        - Logs unknown packet types.
    5. Gracefully shuts down and exits the thread.
    """
    # Setup
    ctx:ManagedPubSubRunnable = getCurrentExecutionContext()

    # Perform any setup here
    setupBarrier.wait() if readyBarrier != None else None
    
    # Nothing to do there
    print(f"Arduino Receive Thread Ready. Publish to {ARDUINO_RECV_TOPIC}? --> {PUBLISH_PACKETS}")   

    # Wait for all Threads ready
    readyBarrier.wait() if readyBarrier != None else None

    # Receiving Logic Loop
    try:
        while(not ctx.isExit()):
            packet = receivePacket(exitFlag=ctx.exitEvent)
            if packet == None:
                # Continue if no packet received
                continue

            # Handle the packet based on its type
            # Default handlers are provided in the control module
            # Pleasee modify or replace to fit the application requirements
            packetType = TPacketType(packet.packetType)
            
            if packetType == TPacketType.PACKET_TYPE_RESPONSE:
                handleResponse(packet, publishPackets = PUBLISH_PACKETS)
            elif packetType == TPacketType.PACKET_TYPE_ERROR:
                handleErrorResponse(packet, publishPackets = PUBLISH_PACKETS)
            elif packetType == TPacketType.PACKET_TYPE_MESSAGE:
                handleMessage(packet, publishPackets = PUBLISH_PACKETS)
            elif packetType == TPacketType.PACKET_TYPE_NEWLINE:
                handleNewline(packet, publishPackets = PUBLISH_PACKETS)
            else:
                print(f"Unknown Packet Type {packetType}")
    except KeyboardInterrupt:
        pass
    except Exception as e:
        print(f"Receive Thread Exception: {e}")
        pass

    # Shutdown and exit the thread gracefully
    ctx.doExit()
    print("Exiting Receive Thread")
    pass

##########################
##### PACKET HANDLERS ####
##########################
def handleResponse(res: TPacket, publishPackets:bool=False):
    """
    Handles the response from the Arduino.

    Args:
        res (TPacket): The response packet received from the Arduino.

    Returns:
        Tuple[TResponseType, Tuple[ctypes.c_uint32]]: A tuple containing the response type and parameters

    Prints:
        - "Command OK" if the response type is RESP_OK.
        - "Status OK" if the response type is RESP_STATUS.
        - "Arduino sent unknown response type {res_type}" for any other response type.
    """
    res_type = TResponseType(res.command)
    if res_type == TResponseType.RESP_OK:
        print("Command OK")

        if publishPackets:
            publish("arduino/recv", (res.packetType, res.command ))

    elif res_type == TResponseType.RESP_STATUS:
        # we assume that the status if stored in the parameters
        # we will print the status
        params = tuple([p for p in res.params])
        status_str = ""
        for idx, p in enumerate(params):
            # We don't know what your parameters are, so we will just print them as is
            # You can modify this to fit your application
            param_name = "param" + str(idx)
            status_str += f"{param_name}: {p}\n"
        print(f"Status OK: \n{status_str}")
        
        if publishPackets:
            publish("arduino/recv", (res.packetType, res.command, params))
    else:
        print(f"Arduino sent unknown response type {res_type}")

def reset_comms_packet(comms_packet: TComms):
    """Completely resets a TComms packet structure"""
    try:
        # Reset header fields
        comms_packet.magic = 0
        comms_packet.dataSize = 0
        comms_packet.checksum = 0
        
        # Clear the buffer
        ctypes.memset(ctypes.addressof(comms_packet.buffer), 0, COMMS_BUFFER_SIZE)
        
        # Reset padding
        comms_packet.dummy = b'\x00\x00\x00'
        return True
    except Exception as e:
        print(f"Comms reset failed: {str(e)}")
        return False

def reset_tpacket(tpacket: TPacket):
    """Completely resets a TPacket structure"""
    try:
        # Numeric fields
        tpacket.packetType = 0
        tpacket.command = 0
        
        # Data buffer
        ctypes.memset(ctypes.addressof(tpacket.data), 0, PAYLOAD_DATA_MAX_STR_LEN)
        
        # Parameters array
        zero_params = (ctypes.c_uint32 * PAYLOAD_PARAMS_COUNT)()
        ctypes.memmove(
            ctypes.addressof(tpacket.params),
            ctypes.addressof(zero_params),
            ctypes.sizeof(zero_params)
        )
        
        # Dummy padding
        tpacket.dummy = b'\x00\x00'
        return True
    except Exception as e:
        print(f"TPacket reset failed: {str(e)}")
        return False

def handleErrorResponse(comms_packet: TComms, publishPackets: bool = False):
    """Handles error responses with proper packet resetting"""
    tpacket = None  # Initialize to None
    
    try:
        # First verify we have a valid comms packet
        if comms_packet.magic != COMMS_MAGIC_NUMBER:
            print("Error: Bad magic number in comms packet")
            reset_comms_packet(comms_packet)
            return
            
        # Extract TPacket from buffer
        try:
            tpacket = ctypes.cast(
                ctypes.addressof(comms_packet.buffer),
                ctypes.POINTER(TPacket)
            ).contents
        except Exception as e:
            print(f"Error: Failed to extract TPacket from buffer: {str(e)}")
            reset_comms_packet(comms_packet)
            return
            
        # Process error type
        try:
            error_msgs = {
                TResponseType.RESP_BAD_PACKET: "Bad packet structure",
                TResponseType.RESP_BAD_CHECKSUM: "Bad checksum", 
                TResponseType.RESP_BAD_COMMAND: "Bad command",
                TResponseType.RESP_BAD_RESPONSE: "Unexpected response"
            }
            msg = error_msgs.get(TResponseType(tpacket.command), "Unknown error")
            print(f"Arduino Error: {msg}")
            
            if publishPackets:
                publish("arduino/recv", (tpacket.packetType, tpacket.command))
                
        except ValueError as e:
            print(f"Error: Invalid response type: {str(e)}")
            
    except Exception as e:
        print(f"Error handling failed: {str(e)}")
        
    finally:
        # Always reset both packets if they exist
        try:
            if tpacket is not None:
                if not reset_tpacket(tpacket):
                    print("Warning: TPacket reset failed, recreating")
                    new_tpacket = TPacket()
                    ctypes.memmove(
                        ctypes.addressof(tpacket),
                        ctypes.addressof(new_tpacket),
                        PAYLOAD_PACKET_SIZE
                    )
        except Exception as e:
            print(f"TPacket reset error: {str(e)}")
            
        try:
            if not reset_comms_packet(comms_packet):
                print("Warning: Comms packet reset failed, recreating")
                new_comms = TComms()
                ctypes.memmove(
                    ctypes.addressof(comms_packet),
                    ctypes.addressof(new_comms),
                    COMMS_PACKET_SIZE
                )
        except Exception as e:
            print(f"Comms packet reset error: {str(e)}")
def handleMessage(res:TPacket, publishPackets:bool=False):
    """
    Handles the incoming message from an Arduino device.

    Args:
        res (TPacket): The packet received from the Arduino, containing the data to be processed.

    Returns:
        None
    """
    message = str(res.data, 'utf-8')
    print(f"{message}", end="")
    if publishPackets:
        publish("arduino/recv", (res.packetType, res.command, message))
    pass

def handleNewline(res:TPacket, publishPackets:bool=False):
    print("")
