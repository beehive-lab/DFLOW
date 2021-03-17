#!/usr/bin/env python3

import os
from binascii import hexlify
#import cantools
#import can
import time

class CAN_Decoder:
    def __init__(self,DBC_FILE_PATH):
        self.database = cantools.db.load_file(DBC_FILE_PATH)
        can.rc['interface'] = 'socketcan'
        can.rc['channel'] = 'vcan0'
        self.can_bus = can.interface.Bus()
        self.engine_message = self.database.get_message_by_name('ENGINE_SENSORS')
        self.imu_message = self.database.get_message_by_name('IMU_SENSOR')
        self.intake_message = self.database.get_message_by_name('INTAKE_SENSORS')
        self.abs_message = self.database.get_message_by_name('ABS_MODULE')
        self.tpm_message = self.database.get_message_by_name('TPM_MODULE')
        self.config_mode_message = self.database.get_message_by_name("CONFIGURABLE_MODES")
        
    def get_message(self):
        message = self.can_bus.recv()
        if(message.arbitration_id == self.imu_message.frame_id):
            decoded = self.database.decode_message(self.imu_message.frame_id, message.data)
            for key in decoded:
                    decoded[key] = str(decoded[key])
            decoded["MessageType"] = "IMUSensor"
            return decoded
        elif(message.arbitration_id == self.intake_message.frame_id):
            decoded = self.database.decode_message(self.intake_message.frame_id, message.data)
            for key in decoded:
                    decoded[key] = str(decoded[key])
            decoded["MessageType"] = "IntakeSensors"
            return decoded
        elif(message.arbitration_id == self.abs_message.frame_id):
            decoded = self.database.decode_message(self.abs_message.frame_id, message.data)
            for key in decoded:
                    decoded[key] = str(decoded[key])
            decoded["MessageType"] = "ABSModule"
            return decoded
        elif(message.arbitration_id == self.tpm_message.frame_id):
            decoded = self.database.decode_message(self.tpm_message.frame_id, message.data)
            for key in decoded:
                    decoded[key] = str(decoded[key])
            decoded["MessageType"] = "TPMModule"
            return decoded
        elif(message.arbitration_id == self.engine_message.frame_id):
            decoded = self.database.decode_message(self.engine_message.frame_id, message.data)
            for key in decoded:
                    decoded[key] = str(decoded[key])
            decoded["MessageType"] = "EngineSensors"
            return decoded
        elif(Message.arbitration_id == self.config_mode_message.frame_id):
            decoded = self.database.decode_message(self.config_mode_message.frame_id, message.data)
            for key in decoded:
                    decoded[key] = str(decoded[key])
            decoded["MessageType"] = "ConfigurableMode"
            return decoded

    def send_config_message(self, abs_mode, tc_mode, throttle_response_mode):
        data = self.config_mode_message.encode({"ABSMode": abs_mode, "TCMode": tc_mode, "ThrottleResponseMode": throttle_response_mode})
        message = can.Message(arbitration_id = self.config_mode_message.frame_id, data = data)
        self.can_bus.send(message)
        time.sleep(0.1)
