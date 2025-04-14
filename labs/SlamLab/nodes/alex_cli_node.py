# This node is an example of a simple publisher monitors the input from the user and publishes commands to the "arduino/send" topic.

# Import Python Native Modules. We require the Barrier class from threading to synchronize the start of multiple threads.
from threading import Barrier
import signal
import time

# Import the required pubsub modules. PubSubMsg class to extract the payload from a message.
from pubsub.pub_sub_manager import ManagedPubSubRunnable, PubSubMsg
from pubsub.pub_sub_manager import publish, subscribe, unsubscribe, getMessages, getCurrentExecutionContext

# Import the command parser from the control module
from control.alex_control import parseUserInput

# Constants
ARDUINO_SEND_TOPIC = "arduino/send"

def cliThread(setupBarrier:Barrier=None, readyBarrier:Barrier=None):
    ctx = getCurrentExecutionContext()
    
    try:
        # Setup synchronization
        if setupBarrier: setupBarrier.wait()
        print(f"CLI Thread Ready. Publishing to {ARDUINO_SEND_TOPIC}")
        if readyBarrier: readyBarrier.wait()

        while not ctx.isExit():
            # Get user input
            input_str = input("Commands (9:move time, 0:turn, 1:open, 2:close, k:dispense, l:retract, q:quit)\n")
            
            # Parse command
            parseResult = parseUserInput(input_str, exitFlag=ctx.exitEvent)
            if not parseResult:
                print("Invalid command")
                continue
            
            # Send command and wait for ACK
            publish(ARDUINO_SEND_TOPIC, tuple(parseResult))
            print(f"Sent: {parseResult[1].name}", end='', flush=True)
            
            # Wait for Arduino response
            ack_received = False
            start_time = time.time()
            while not ack_received and not ctx.isExit():
                # Check for response with timeout
                response = getMessages("arduino/response", timeout=0.1)
                if response:
                    print(f" | Arduino ACK: {response[-1][1].name}")
                    ack_received = True
                elif time.time() - start_time > 1:  #  1 sec timeout
                    print(" | Timed out waiting for response!")
                    break
                
                # Visual waiting indicator
                print(".", end='', flush=True)
            
    except Exception as e:
        print(f"CLI Thread Exception: {e}")
    finally:
        ctx.doExit()
        print("Exiting Command Thread")