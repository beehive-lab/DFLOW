#Author:Radu-Tudor Andra
import can
import cantools
import time
from pprint import pprint
db = cantools.database.load_file('./DFLOW.dbc')
db.messages

tpm_message = db.get_message_by_name('TPM_MODULE')
tyrePressF = 2.5
tyrePressR = 2.8


can_bus = can.interface.Bus('vcan0', bustype='socketcan')
while(True):

	tpm_data = tpm_message.encode({'TyrePressureRear': tyrePressR, 'TyrePressureFront': tyrePressF})
	send_tpm_message = can.Message(arbitration_id = tpm_message.frame_id, data = tpm_data)
	can_bus.send(send_tpm_message)

	time.sleep(2)
