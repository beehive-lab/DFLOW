#Author:Radu-Tudor Andra
import can
import cantools
import time
from pprint import pprint
db = cantools.database.load_file('./DFLOW.dbc')
db.messages

abs_message = db.get_message_by_name('ABS_MODULE')
motorcycleSpeed = 0
rearWheelSpeed = 0
frontWheelSpeed = 0
brakeRearActive = 0
brakeFrontActive = 0

can_bus = can.interface.Bus('vcan0', bustype='socketcan')
while(True):
	motorcycleSpeed +=1
	if(motorcycleSpeed > 300):
	    motorcycleSpeed = 0

	rearWheelSpeed +=1
	if(rearWheelSpeed > 6000):
	    rearWheelSpeed = 0

	frontWheelSpeed +=1
	if(frontWheelSpeed > 6000):
	    frontWheelSpeed = 0

	if(brakeRearActive  == 1):
	    brakeRearActive = 0
	else:
		brakeRearActive = 1
	
	if(brakeFrontActive == 1):
	    brakeFrontActive = 0
	else:
		brakeFrontActive = 1

	abs_data = abs_message.encode({'MotorcycleSpeed': motorcycleSpeed, 'RearWheelSpeed': rearWheelSpeed, 'FrontWheelSpeed': frontWheelSpeed, 'BrakeRearActive': brakeRearActive, 'BrakeFrontActive': brakeFrontActive})
	send_abs_message = can.Message(arbitration_id = abs_message.frame_id, data = abs_data)
	can_bus.send(send_abs_message)

	time.sleep(0.05)
