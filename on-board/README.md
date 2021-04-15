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
