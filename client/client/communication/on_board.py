from client.communication.messages import (
    BikeSensorDataKey,
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

    def start_streaming_data(self, data_keys: [BikeSensorDataKey]) -> None:
        msg_command: MessageCommand = MessageCommand.STREAM_BIKE_SENSOR_DATA
        msg_args: [str] = [str(data_key) for data_key in data_keys] + ['start']

        message: str = build_message(
            msg_command,
            msg_args
        )

        self._comm_link.send(message.encode())

    def stop_streaming_data(self, data_keys: [BikeSensorDataKey]) -> None:
        msg_command: MessageCommand = MessageCommand.STREAM_BIKE_SENSOR_DATA
        msg_args: [str] = [str(data_key) for data_key in data_keys] + ['stop']

        message = build_message(
            msg_command,
            msg_args
        )

        self._comm_link.send(message.encode())
