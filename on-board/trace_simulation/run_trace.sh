#!/bin/bash

python DFLOW_trace_simulator_ConfigModes_ECU.py &

python DFLOW_trace_simulator_ABS_ECU.py &

python DFLOW_trace_simulator_Engine_ECU.py &

python DFLOW_trace_simulator_TPM_ECU.py &

python DFLOW_trace_simulator_IMU_ECU.py &

python DFLOW_trace_simulator_Intake_ECU.py &