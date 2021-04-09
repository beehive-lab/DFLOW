import socket
from threading import Thread

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

    def __init__(self, comm_link: CommLink):
        self._comm_link = comm_link

    def get_incoming_msg(self) -> bytes:
        return self._comm_link.receive()

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

    def disconnect(self):
        self._comm_link.disconnect()

    def is_connected(self):
        return self._comm_link.is_connected()


class IncomingMessageHandler(Thread):
    """
    Responsible for handling incoming messages from on_board and storing
    any received data asynchronously.
    """

    _SENSOR_DATA_KEY = 'sensor_data'

    def __init__(self, on_board: OnBoard) -> None:
        super().__init__()
        self._on_board: OnBoard = on_board
        self._running = True
        self.received_data: dict[str, dict[str, list[tuple[bytes, bytes]]]] = \
            dict()
        self.received_data[self._SENSOR_DATA_KEY] = dict()

    def run(self) -> None:
        while self._running:
            if self._on_board.is_connected():
                try:
                    incoming_msg: bytes = self._on_board.get_incoming_msg()
                    self._handle_incoming_messages(incoming_msg)
                except socket.timeout:
                    # If timeout then just try again.
                    continue

    def _handle_incoming_messages(self, msg: bytes) -> None:
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

    def stop(self):
        self._running = False
