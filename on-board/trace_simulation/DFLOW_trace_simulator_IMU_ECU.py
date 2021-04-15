#Author:Radu-Tudor Andra
import can
import cantools
import time
from pprint import pprint
db = cantools.database.load_file('./DFLOW.dbc')
db.messages

imu_message = db.get_message_by_name('IMU_SENSOR')
leanAngle = 0

can_bus = can.interface.Bus('vcan0', bustype='socketcan')
while(True):
	leanAngle += 0.1 
	if(leanAngle > 45):
	    leanAngle = -45

	imu_data = imu_message.encode({'LeanAngle': leanAngle})
	send_imu_message = can.Message(arbitration_id = imu_message.frame_id, data = imu_data)
	can_bus.send(send_imu_message)

	time.sleep(0.01)
