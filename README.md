# MEMS Indoor Tracking

This repository contains documentation, firmware, software, test plans, and test results for a Master's Thesis. The subject of the Thesis is "Movement tracking system based on micromechanical sensors". 

The main tasks of the thesis include:

* Research on passive navigation methods.
* Selection of appropriate sensors, microcontroller and communication protocols.
* Implementation of computing algorithm based on sensor data.
* Development of desktop or mobile application for gathered data visualization.
* Full system integration and testing.

## Concept
The primary objective of this project is to implement and evaluate various dead reckoning and displacement estimation methods using micromechanical sensors. The system is designed to acquire inertial data in real time, stream it wirelessly or via serial link to a dedicated host application for live visualization and telemetry logging, and facilitate comparative experimental studies to determine the accuracy, drift characteristics, and practical limitations of each positioning approach.

### Firmware
The firmware is build on ESP-IDF framework in C, leveraging FreeRTOS for task management. The system employs modular architecture to separate low-level control from data processing required for passive navigation.

#### System Layers:
* Application layer [`app`](./firmware/src/app/) - System lifecycle, command parsing annd RTOS task initialization.
* Network Stack [`network`](./firmware/src/network/) - Wireless connectivity and data streaming for user interface application.
* Device Drivers [`devices`](./firmware/src/devices/) - Agnostic sensor configuration, register mapping and data frame formatting.
* Hardware Abstraction [`peripherals`](./firmware/src/peripherals/), [`hal`](./firmware/src/hal/)- MCU-specific peripheral bindings and communication wrappers.

#### Data Flow & Execution Pipeline
* Dedicated tasks are defined to handle specific sensor submodule initialization, polling loops, and data formatting.
* Control commands are ingested via dedicated port into non-blocking ring buffers and routed by command dispatcher.
* Data from selected sensor is continuosly read via I2C bus.
* Formatted data is continuously streamed back using dedicated port.

### Software
The software is a multithreaded Python application designed to interface with the tracking hardware. It provides a control terminal, data logging, and an extensible foundation for a graphical visualization layer.

#### System Layers:
* User Interface [`ui`](./software/src/ui/) - Control Interface featuring CLI build on `cmd.Cmd` for commands and streaming telemetry. In the future will contain integration with dedicated graphical layer.
* Communication [`communication`](./software/src/communication/) - Base communication interface and implementation for Serial (UART) and UDP protocols.
* Utilites [`utils`](./software/src/utils/) - Application runtime helpers (Argument handling and parsing, JSON configuration, logging).

## Technologies Used
* Espressif ESP32-D0WDQ6 (from ESP32WROOM32 devkit)
* STMicroeletrconics LSM6DSOX
* CEVA BNO085
* I2C, USB-UART
* UDP
* C11
* ESP-IDF v6.1.0
* FreeRTOS
* Python 3.14
* PySide6

## Directories descriptions
* [`firmware`](./firmware) - contains the complete ESP-IDF project source code.
* [`software`](./software) - contains the User Interface application made in Python.

## License
This project is proprietary and currently not licensed for open-source use. All rights reserved.
