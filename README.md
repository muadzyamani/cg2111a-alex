# Alex Robotics System

## Overview
The Alex Robotics System is a multi-threaded and multi-process project designed to control an Arduino-based robot, integrate Lidar scanning, perform SLAM (Simultaneous Localization and Mapping), manage TLS network relays, and provide a user interface for monitoring and controlling the robot.

## Features
- **Arduino Communication**: Send and receive packets to/from the Arduino for robot control.
- **Lidar Integration**: Real-time Lidar scanning and data visualization.
- **SLAM**: Simultaneous Localization and Mapping for robot navigation.
- **Pub/Sub System**: Publish/Subscribe messaging for inter-thread/process communication.
- **TLS Relay**: Secure communication via TLS for remote control.
- **GUI**: Real-time visualization of Lidar and SLAM data.


## Setup Instructions
1. Clone the repository to your local machine.
2. Run the `setup_environment.sh` script to ensure all dependencies are installed.
   ```bash
   ./setup_environment.sh
   ```
3. Activate the Python virtual environment:
    ```bash
    source ./env/bin/activate
    ```
4. Navigate to the `labs/SlamLab` directory:
    ```bash
    python alex_main.py
    ```

## Dependencies
The project requires the following Python libraries:

- numpy
- matplotlib
- pyserial
- breezyslam
- pyrplidar
- Additional libraries for networking and display are included in the `libraries/epp2` folder.

## Key Files
- [alex_main.py](labs/SlamLab/alex_main.py): Entry point for the system.
- [alex_arduino_receive_node.py](labs/SlamLab/nodes/alex_arduino_receive_node.py): Handles incoming packets from the Arduino.
- [alex_display_node.py](labs/SlamLab/nodes/alex_display_node.py): Visualizes Lidar and SLAM data using Matplotlib.
- [alex_slam_node.py](labs/SlamLab/nodes/alex_slam_node.py): Processes Lidar scans and updates SLAM.
- [alex_bokeh_display_node.py](labs/SlamLab/nodes/alex_bokeh_display_node.py): Alternative visualization

## Running on Raspberry Pi
1. SSH into the Raspberry Pi
    ```bash
    ssh pi@<IP_ADDRESS>
    ```
    Enter password.
2. Navigate to the project directory:
    ```bash
    cd slam/studio/alex/alex/
    ```
3. Follow the setup instructions above.

## Authors
Developed as part of the CG2111A Engineering Principles and Practices II course at NUS.

## Video
The final run can be watched [here](https://www.youtube.com/watch?v=BeZuBEpBcQ8).