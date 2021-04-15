#Author:Radu-Tudor Andra
import can
import cantools
import time
from pprint import pprint
db = cantools.database.load_file('./DFLOW.dbc')
db.messages
intake_message = db.get_message_by_name('INTAKE_SENSORS')
airTemp = 30.1
throttlePos = 0

can_bus = can.interface.Bus('vcan0', bustype='socketcan')
while(True):
	throttlePos += 1
	if(throttlePos > 100):
		throttlePos = 0

	intake_data = intake_message.encode({'AirTemperature': airTemp, 'ThrottlePosition': throttlePos})
	send_intake_message = can.Message(arbitration_id= intake_message.frame_id, data=intake_data)
	can_bus.send(send_intake_message)

	time.sleep(0.2)
