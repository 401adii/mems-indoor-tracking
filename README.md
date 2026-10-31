# MEMS Indoor Tracking

This repository contains documentation, firmware, software, test plans, and test results for a Master's Thesis. The subject of the Thesis is "Movement tracking system based on micromechanical sensors". 

The main tasks of the thesis include:

* Research on passive navigation methods.
* Selection of appropriate sensors, microcontroller, and communication protocols.
* Implementation of computing algorithm based on a sensor data.
* Development of desktop or mobile application for visualizing acquired data.
* Full system integration and testing.

## Concept
The primary objective of this project is to implement and evaluate various dead reckoning and displacement estimation methods using micromechanical sensors. The system acquires inertial sensor data in real time. A dedicated host application then receives the wireless or serial stream to handle live visualization and telemetry logging. Comparative experimental studies will be conducted to determine the accuracy, drift characteristics, and practical limitations of each positioning approach.

### Firmware
The firmware is built on ESP-IDF framework in C, leveraging FreeRTOS for task management. The system employs a modular architecture to separate low-level control from data processing required for passive navigation.

#### System Layers:
* Application layer [`app`](./firmware/src/app/) - System lifecycle, command parsing and RTOS task initialization.
* Network Stack [`network`](./firmware/src/network/) - Wireless connectivity and data streaming for user interface application.
* Device Drivers [`devices`](./firmware/src/devices/) - Agnostic sensor configuration, register mapping and data frame formatting.
* Hardware Abstraction [`peripherals`](./firmware/src/peripherals/), [`hal`](./firmware/src/hal/) - MCU-specific peripheral bindings and communication wrappers.

#### Data Flow & Execution Pipeline
* Dedicated tasks are defined to handle specific sensor submodule initialization, polling loops, and data formatting.
* Control commands are ingested via a dedicated port into non-blocking ring buffers and routed by a command dispatcher.
* Data from selected sensor is continuously read via I2C bus.
* Formatted data is continuously streamed back using a dedicated port.

### Software
The software is a multithreaded Python application designed to interface with the tracking hardware. It provides a control terminal, data logging, and an extensible foundation for a graphical visualization layer.

#### System Layers:
* Core [`core`](./software/src/core/) - Bridges translating telecommands and telemetry.
* User Interface [`ui`](./software/src/ui/) - Graphical Interface build using PySide6 library.
* Communication [`communication`](./software/src/communication/) - Base communication interface and implementation for Serial (UART) and UDP protocols.
* Utilities [`utils`](./software/src/utils/) - Application runtime helpers (Argument handling and parsing, JSON configuration, logging).

## User Manual
### Firmware & Hardware Setup
To flash the firmware onto the ESP32 MCU, install the [ESP-IDF](https://docs.espressif.com/projects/esp-idf/en/stable/esp32/get-started/index.html#installation) framework. The device firmware is built and validated using ESP-IDF v6.1.

> **TODO:** add hardware pinout.

### Software Setup

To set up software enter [`software/`] directory. Virtual environment and dependencies installation can be done using setup shell script:
```bash
./scripts/setup.sh
```

### Software Launch
The application resolves connection settings using a strict hierarchy: command-line arguments, a local configuration file, or an interactive GUI prompt. 
* Command-LineOverrides: Launch the application with arguments to instantly connect and bypass the setup screen.
    * *Serial*: `python3 main.py --serial-port /dev/ttyUSB0 --baudrate 115200`
    * *UDP*: `python3 main.py --udp-listen-port 4554` 
* Config File: Place a configuration file in the root directory to define default fallback parameters for headless or repeated launches.
* Connection Panel: If you run `python3 main.py` without CLI arguments and no valid configuration file is found, the GUI Connection Window appears. This panel allows you to dynamically scan for active USB COM ports, configure UDP target IPs and listen ports, and toggle background data logging before establishing a connection.

### Application Workflow
* Use the radio buttons in the Configuration Panel to designate the active hardware module (LSM6DSOX or BNO085).
* Click the Method dropdown to select a specific tracking algorithm.
#### Execution Controls:
    * **Start**: Commands the ESP32 to lock in the current output mode and begin continuously transmitting computed telemetry.
    * **Stop**: Halts the data stream.
    * **Restart MCU**: Located at the bottom of the panel, this button sends a forced reboot request to the ESP32 hardware.

## Technologies Used
* Espressif ESP32-D0WDQ6 (from ESP32-WROOM-32 devkit)
* STMicroelectronics LSM6DSOX
* CEVA BNO085
* I2C, USB-UART
* UDP
* C11
* ESP-IDF v6.1.0
* FreeRTOS
* Python 3.14
* PySide6

## Directory Descriptions
* [`firmware`](./firmware) - contains the complete ESP-IDF project source code.
* [`software`](./software) - contains the User Interface application made in Python.

## License
This project is proprietary and currently not licensed for open-source use. All rights reserved.