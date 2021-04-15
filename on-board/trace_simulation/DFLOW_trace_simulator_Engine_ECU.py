#Author:Radu-Tudor Andra
import can
import cantools
import time
from pprint import pprint
db = cantools.database.load_file('./DFLOW.dbc')
db.messages

engine_message = db.get_message_by_name('ENGINE_SENSORS')
voltage = 13.1
oilPressure = 1.9
gearPosition = 0
waterTemp = 80
engineSpeed = 2000

eSChange = 1
vChange = 1
oChange = 1
gpChange = 1
wtChange =1

can_bus = can.interface.Bus('vcan0', bustype='socketcan')
while(True):
	engineSpeed += 100 * eSChange
	if(engineSpeed > 8000):
	        eSChange = -1
	elif(engineSpeed < 2000):
			eSChange = 1

	voltage += 0.01 * vChange
	if(voltage > 13.7):
		vChange = -1
	elif(voltage < 13.1):
		vChange = 1

	oilPressure += 0.01 * oChange
	if(oilPressure > 2.5):
		oChange = -1
	elif(oilPressure < 1.9):
		oChange = 1

	gearPosition += 1 * gpChange
	if(gearPosition > 5):
		gpChange = -1
	if(gearPosition < 1):
		gpChange = 1

	waterTemp += 0.1 * wtChange
	if(waterTemp > 86):
		wtChange = -1
	if(waterTemp < 80):
		wtChange = 1

	engine_data = engine_message.encode({'BatteryVoltage': voltage, 'OilPressure': oilPressure, 'GearPosition': gearPosition, 'WaterTemperature': waterTemp, 'EngineSpeed': engineSpeed})
	send_engine_message = can.Message(arbitration_id = engine_message.frame_id, data = engine_data)
	can_bus.send(send_engine_message)

	time.sleep(0.1)
