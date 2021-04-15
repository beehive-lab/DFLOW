Authors of System
------------------
Radu-Tudor Andra: `src/can_module`, `src/data_processing_module`, `src/udf_module`, `src/profiling_module`, `on-board_benchmarking`, `trace_simulation`, and any respective tests inside `test`
Maximilian Gama: `src/inter-comms`, `src/crypto` and any respective tests inside `test`

System parameters
-----------------
* CAN-module : only parameters here are the DBC file path, path of the python module implementing decoding, and an optional file path for the file data interface.
* Data-Processing-module : a vector providing the desired processing operation for each individual message identifier(whose values can be found in `src/data_processing_module/pipes.cpp`). At the moment every signal is set to Average over buffer however the possible processing functions are: AVERAGE_OF_BUFFER, MAX_OF_BUFFER, MIN_OF_BUFFER, SUM_OFF_BUFFER, FULL_BUFFER, DO_NOT_COMPUTE. 
  
  Also other parameters of the module in the order they are required by the constructor are: the tick interval(at which rate values will be pushed to buffers), time_interval(at which rate buffers are offloaded to pipes) and buffer size).
* UDF-module : the parameters that can and must be passed to the UDF module's AI function, besides the instance of on-board data interface are: a vector containing the desired input signal identifiers, another vector containing their type, the path to the json frugally-deep DNN model. 

Mention regarding the Profiling module
--------------------------------------
The Profiling module is designed to be instantiated only once initially and then be run in a loop to compute CPU Usage, either through `computeAndSendStats()`, sending the profiling data as an on-board signal, or through `getCPUUsage()`, retrieving the percentage locally, to be used directly on the device running the on-board system.

Different main functions and build targets
-------------------------------------------
There are 3 different main functions and main build targets:
* mainRadu.cpp -> Radu_DFLOW_OnBoard - designed to showcase Radu's contribution and run the on-board system without communication to the client
* main.cpp -> DFLOW_OnBoard designed to provide full functionality of the system, providing Maxi's contribution on top of Radu's work
* mainWithAi.cpp -> DFLOW_OnBoard_WithAi - designed to provide the edge AI functionality, it has been separated into a different build as frugally-deep takes longer to compile.

Modifying system/CAN messages/signals
-------------------------------------

All the current messages and signals identifiers are defined in `src/data_processing_module/pipes.hpp` and declared in `src/data_processing_module/pipes.hpp`, including their total number.

In order to add remove or modify signals one must:
1. Replace the `DFLOW.dbc` message database file inside `src/can_module/` with one holding the appropriate messages.
2. Add any new messages to the python file `python_module.py` from `src/can_module/` following cantools' documentation.
3. Create a message data-class inside `src/can_module/messages/` similar to the rest and inside of it a function mapping a `std::map` containing the signal identifiers from the dbc file as keys and mapping the values to the classes data values.
4. Add/Remove/Modify the identifiers inside `src/data_processing_module/pipes.cpp` & `src/data_processing_module/pipes.hpp` and their number
5. Inside `src/can_module/can_module.cpp` link the values from the map retrieved by the CAN module to a message class instance and send it through the pipes.
6. Modify the Data Processing module, changing the contents of the `processing_inputs` & `processing_inputs_type` vectors. If new messages were added, define them in the header like the rest and modify the mapping inside the `readCanPipes()` and `pushBackToBuckets()` functions.
7. Now add the signals to the onboard interface inside `src/udf-module/on_board_data_interface.hpp` & `src/udf-module/on_board_data_interface.cpp`
8. Implement any desired inter-connectivity with the client inside `src/inter-comms/Logic` utilising the on-board data interface.

Now the on-board system should be tailored to the Controller Area Network it operates over.
