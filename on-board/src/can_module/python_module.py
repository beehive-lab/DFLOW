#!/usr/bin/env python3

import os
from binascii import hexlify
import cantools
import can

class CAN_Decoder:
    def __init__(self,DBC_FILE_PATH):
        self.database = cantools.db.load_file(DBC_FILE_PATH)
        can.rc['interface'] = 'socketcan'
        can.rc['channel'] = 'vcan0'
        self.can_bus = can.interface.Bus()
        
    def get_example_message(self):
        self.example_message = self.database.get_message_by_name('ExampleMessage')
        message = self.can_bus.recv()
        decoded = self.database.decode_message(self.example_message.frame_id, message.data)
        return decoded["AverageRadius"]
