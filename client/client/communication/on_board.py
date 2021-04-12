import socket
from threading import Thread
from time import sleep

from client.communication.messages import (
    SensorDataKey,
    MessageCommand,
    build_message
)
from client.interconnect.commlink import CommLink


class OnBoard:
    """
    Represents the on-board module.
    """

    _SENSOR_DATA_KEY = 'sensor_data'

    def __init__(self, comm_link: CommLink):
        self._comm_link = comm_link
        self._comm_link.connect()

        self.received_data: dict[str, dict[str, list[tuple[bytes, bytes]]]] = \
            dict()
        self.received_data[self._SENSOR_DATA_KEY] = dict()

        self._msg_handler = IncomingMessageHandler(self, comm_link)
        self._msg_handler.start()

    def __enter__(self):
        return self

    def __exit__(self, exc_type, exc_value, exc_traceback):
        self.exit()

    def exit(self):
        self._msg_handler.stop()
        self._msg_handler.join()
        self._comm_link.disconnect()

    # ************** PROCESS CLIENT COMMANDS **************

    def start_streaming_data(self, data_keys: [SensorDataKey]) -> None:
        msg_command: MessageCommand = MessageCommand.STREAM_BIKE_SENSOR_DATA
        msg_args: [str] = ['start'] + [str(data_key) for data_key in data_keys]

        message: str = build_message(
            msg_command,
            msg_args
        )

        self._comm_link.send(message.encode())

    def stop_streaming_data(self, data_keys: [SensorDataKey]) -> None:
        msg_command: MessageCommand = MessageCommand.STREAM_BIKE_SENSOR_DATA
        msg_args: [str] = ['stop'] + [str(data_key) for data_key in data_keys]

        message = build_message(
            msg_command,
            msg_args
        )

        self._comm_link.send(message.encode())

    def is_connected(self):
        return self._comm_link.is_connected()

    def enable_secure_comms(self):
        message = build_message(MessageCommand.ENCRYPTION, ['on'])
        self._comm_link.send(message.encode())
        sleep(1)
        self.reconnect(True)

    def disable_secure_comms(self):
        message = build_message(MessageCommand.ENCRYPTION, ['off'])
        self._comm_link.send(message.encode())
        sleep(1)
        self.reconnect(False)

    def reconnect(self, secure=None):
        self._comm_link.reconnect(secure)

    # ************** PROCESS INCOMING MESSAGES FROM ONBOARD **************

    def handle_incoming_message(self, msg: bytes) -> None:
        if msg.startswith(b'stream-bike-sensor-data'):
            msg_type, *parts = msg.split(b':')
            for i in range(0, len(parts), 3):
                self._record_received_sensor_data(
                    parts[i].decode(),
                    parts[i + 1],
                    parts[i + 2]
                )
        else:
            raise Exception('Unknown message type received: ' + msg.decode())

    def _record_received_sensor_data(
        self,
        data_type: str,
        timestamp: bytes,
        data_value: bytes,
    ) -> None:
        if data_type not in self.received_data[self._SENSOR_DATA_KEY]:
            self.received_data[self._SENSOR_DATA_KEY][data_type] = []

        self.received_data[self._SENSOR_DATA_KEY][data_type].append(
            (timestamp, data_value)
        )

    def get_recorded_sensor_data(self, data_type: SensorDataKey) -> []:
        if data_type in self.received_data[self._SENSOR_DATA_KEY]:
            return self.received_data[self._SENSOR_DATA_KEY][str(data_type)]
        else:
            return []


class IncomingMessageHandler(Thread):
    """
    Responsible for continuously polling socket for new data from on_board.
    """

    def __init__(self, on_board: OnBoard, comm_link: CommLink) -> None:
        super().__init__()
        self._on_board: OnBoard = on_board
        self._comm_link = comm_link
        self._running = True

    def run(self) -> None:
        while self._running:
            if self._comm_link.is_connected():
                try:
                    incoming_msg: bytes = self._comm_link.receive()
                    self._on_board.handle_incoming_message(incoming_msg)
                except socket.timeout:
                    # If timeout then just try again.
                    continue

    def stop(self):
        self._running = False
