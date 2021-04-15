#Author:Radu-Tudor Andra
import can
import cantools
import time
from pprint import pprint
db = cantools.database.load_file('./DFLOW.dbc')
db.messages
config_message = db.get_message_by_name('CONFIGURABLE_MODES')
absMode = 0
tcMode = 7
trMode = 0

absChange = 1
tcChange = -1
trChange = 1

can_bus = can.interface.Bus('vcan0', bustype='socketcan')
while(True):
    absMode += absChange
    if(absMode > 6):
        absChange = -1
    elif(absMode < 1):
        absChange = 1

    tcMode += tcChange
    if(tcMode > 6):
        tcChange = -1
    elif(tcMode < 1):
        tcChange = 1

    trMode += trChange
    if(trMode > 6):
        trChange = -1
    elif(trMode < 1):
        trChange = 1

    config_data = config_message.encode({'ABSMode': absMode, 'TCMode':tcMode, 'ThrottleResponseMode': trMode})
    send_config_message = can.Message(arbitration_id = config_message.frame_id, data = config_data)
    can_bus.send(send_config_message)

    time.sleep(0.9)
