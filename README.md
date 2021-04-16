# DFLOW
[![Build Status](https://travis-ci.com/beehive-lab/DFLOW.svg?token=qDp8gaGxvVS2UKxxk2f5&branch=main)](https://travis-ci.com/beehive-lab/DFLOW)

**A framework for rapidly prototyping data flow proof-of-concepts**

Participants: Maximilian Gama, Osher Landes, Radu-Tudor Andra
==================

This framework provides the means of accessing a CAN-based system(Motorcycle signals have been used in this case) and utilizing it's resources for IoT applications. The project can be used either as an example, a platform which allows for prototyping and assesment, or a starting point for a more complex project.

------------------
### Components:
 - On-Board system: retrieves data from the CAN bus(file data interface is also possible) and performs signal synchronisation and data processing operations so it can be easily and efficiently accesed and either perform local function or send over to the client through secure Wifi or Bluetooth.
 - Client system: retrieves and sends data from and to the on-board securely, providing tools for data analysis.


------------------
### Functionalities provided:

On-Board:
 - Controller Area Network(CAN) module: allows CAN message encoding and decoding through Python embedding and use of cantools;
 - Data Processing module: performs synchronization of different message rates through bucketing up to the desired rate, provides data separation into multiple streams as well as individual buffering and allows for processing operations on the buffers prior to sending to reduce communication loads; 
 - User Defined Functions(UDF) module: provides an interface for easy access to the on-board signals, allows signal selection and injection of frugaly-deep converted Keras DNNs for prediction supporting run-time interchangeability;
 - Profiling module: performs metric checks such as CPU and memory usage, CPU frequency and temperature

Client:
 - Stream sensor data from the on-board 
 - Stream profiling data from the on-board
 - Plot graphs of recorded profiling and sensor data over time
 - Stream graphs of live profiling and sensor data in dashboard
 - Toggle encryption settings
 - Test throughput between client & on-board
 - Send requests to on-board to do performance benchmarks

The library is designed to work with Linux but can be adapted to other OSs.

### Getting started

Libraries required
-------------------------
* cmake
* libssl-dev
* libboost-all-dev
* libbluetooth-dev
* python 3.8+
* cantools
* frugally-deep

cantools can be installed using the `requirements.txt` file in `on-board/trace_simulation` by running `python install -r requirements.txt`. Similarly, frugally-deep can be installed running the `isntall_frugally-deep.sh` script in `on-board`.

cantools repository: https://github.com/eerimoq/cantools
frugally-deep repository: https://github.com/Dobiasd/frugally-deep

Running the on-board system
-----------------------
1. Compile the project using `cmake build/`, `cd build` and the `cmake --build .` .
2. To run the on-board a CAN bus must be simulated on the machine: the script for this is `can_bus_setup.sh` in `on-board/trace_simulation`.
3. In order to receive data the simulated trace must be run with `run_trace.sh` in `on-board/trace_simulation`.
4. The on-board only functionality can now be run with `./Radu_DFLOW_OnBoard` from inside the on-boards' build folder.
5. For integration of inter-communication functionality with `DFLOW_OnBoard` Test certificates must be generated using `generate_test_certs` and the project path must be set up in `on-board/src/config.cpp`.
6. The full on-board functionality can now be run with `./DFLOW_OnBoard` from inside the on-boards' build folder.


Running the client
-----------------------
To setup and run the client simply cd into the `client` directory and run the `run` script.
