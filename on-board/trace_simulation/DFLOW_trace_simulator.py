#Author:Radu-Tudor Andra
import can
import cantools
import time
from pprint import pprint
db = cantools.database.load_file('./DFLOW.dbc')
db.messages
#[message('ExampleMessage', 0x1f0, False, 8, 'Example message used as template in MotoHawk models.')]
intake_message = db.get_message_by_name('INTAKE_SENSORS')
airTemp = 30.1
throttlePos = 0

tpm_message = db.get_message_by_name('TPM_MODULE')
tyrePressF = 2.5
tyrePressR = 2.8

abs_message = db.get_message_by_name('ABS_MODULE')
motorcycleSpeed = 0
rearWheelSpeed = 0
frontWheelSpeed = 0
brakeRearActive = 0
brakeFrontActive = 0

imu_message = db.get_message_by_name('IMU_SENSOR')
leanAngle = 0

engine_message = db.get_message_by_name('ENGINE_SENSORS')
voltage = 13.7
oilPressure = 2.0
gearPosition = 0
waterTemp = 80
engineSpeed = 2000

config_message = db.get_message_by_name('CONFIGURABLE_MODES')
absMode = 1
tcMode = 3
trMode = 2

can_bus = can.interface.Bus('vcan0', bustype='socketcan')
while(True):
	throttlePos += 1
	if(throttlePos > 100):
		throttlePos = 0

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

	leanAngle += 1
	if(leanAngle == 45):
	    leanAngle = -45

	engineSpeed += 10
	if(engineSpeed == 8000):
	        engineSpeed = 2000

	intake_data = intake_message.encode({'AirTemperature': airTemp, 'ThrottlePosition': throttlePos})
	send_intake_message = can.Message(arbitration_id= intake_message.frame_id, data=intake_data)
	can_bus.send(send_intake_message)

	tpm_data = tpm_message.encode({'TyrePressureRear': tyrePressR, 'TyrePressureFront': tyrePressF})
	send_tpm_message = can.Message(arbitration_id = tpm_message.frame_id, data = tpm_data)
	can_bus.send(send_tpm_message)

	abs_data = abs_message.encode({'MotorcycleSpeed': motorcycleSpeed, 'RearWheelSpeed': rearWheelSpeed, 'FrontWheelSpeed': frontWheelSpeed, 'BrakeRearActive': brakeRearActive, 'BrakeFrontActive': brakeFrontActive})
	send_abs_message = can.Message(arbitration_id = abs_message.frame_id, data = abs_data)
	can_bus.send(send_abs_message)

	imu_data = imu_message.encode({'LeanAngle': leanAngle})
	send_imu_message = can.Message(arbitration_id = imu_message.frame_id, data = imu_data)
	can_bus.send(send_imu_message)

	engine_data = engine_message.encode({'BatteryVoltage': voltage, 'OilPressure': oilPressure, 'GearPosition': gearPosition, 'WaterTemperature': waterTemp, 'EngineSpeed': engineSpeed})
	send_engine_message = can.Message(arbitration_id = engine_message.frame_id, data = engine_data)
	can_bus.send(send_engine_message)
	
	config_data = config_message.encode({'ABSMode': absMode, 'TCMode':tcMode, 'ThrottleResponseMode': trMode})
	send_config_message = can.Message(arbitration_id = config_message.frame_id, data = config_data)
	can_bus.send(send_config_message)

	time.sleep(0.1)
