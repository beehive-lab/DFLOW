from enum import Enum
from threading import Thread
from time import time

from client.communication.messages import (
    SensorDataKey,
    MessageCommand,
    build_command_message_with_args
)
from client.interconnect.commlink import CommLink


class BandwidthTestingStatus(Enum):
    SENDING = 1
    AWAITING_CONFIRMATION = 2
    CONFIRMED = 3


class OnBoard:
    """
    Represents the on-board module.
    """

    _SENSOR_DATA_KEY = 'sensor_data'

    def __init__(self, comm_link: CommLink):
        # The communications link to the on-board.
        self._comm_link: CommLink = comm_link
        self._comm_link.connect()

        # Variables used for bandwidth testing.
        self._bandwidth_testing_status: BandwidthTestingStatus = None
        self._received_bandwidth_testing_data_count = 0

        # Data received from the on-board.
        self.received_data: dict[str, dict[str, list[tuple[bytes, bytes]]]] = \
            dict()
        self.received_data[self._SENSOR_DATA_KEY] = dict()

        # Start the message handler that polls for messages from the on-board.
        self._msg_handler = IncomingMessageHandler(self, comm_link)
        self._msg_handler.start()

    def __enter__(self):
        return self

    def __exit__(self, exc_type, exc_value, exc_traceback):
        self.exit()

    def exit(self):
        self._comm_link.disconnect()
        self._msg_handler.stop()
        self._msg_handler.join()

    # ************** PROCESS CLIENT COMMANDS **************

    def start_streaming_data(self, data_keys: [SensorDataKey]) -> None:
        msg_command: MessageCommand = MessageCommand.STREAM_BIKE_SENSOR_DATA
        msg_args: [str] = ['start'] + [str(data_key) for data_key in data_keys]

        message: str = build_command_message_with_args(
            msg_command,
            msg_args
        )

        self._comm_link.send(message.encode())

    def stop_streaming_data(self, data_keys: [SensorDataKey]) -> None:
        msg_command: MessageCommand = MessageCommand.STREAM_BIKE_SENSOR_DATA
        msg_args: [str] = ['stop'] + [str(data_key) for data_key in data_keys]

        message = build_command_message_with_args(
            msg_command,
            msg_args
        )

        self._comm_link.send(message.encode())

    def enable_secure_comms(self):
        message = build_command_message_with_args(
            MessageCommand.ENCRYPTION, ['on']
        )
        self._comm_link.send(message.encode())

        self.reconnect(True)

    def disable_secure_comms(self):
        message = build_command_message_with_args(
            MessageCommand.ENCRYPTION, ['off']
        )
        self._comm_link.send(message.encode())
        self.reconnect(False)

    def reconnect(self, secure=None):
        self._comm_link.reconnect(secure)

    def perform_throughput_test(self):
        print('Starting throughput test...')
        print('testing throughput to client...')

        # Signal start of bandwidth test to on-board.
        self._comm_link.send(
            str(MessageCommand.BANDWIDTH_TEST_START).encode()
        )

        # Get buffer size and data chunks.
        buffer_size = 1024
        data_chunks_to_send: int = 1000000

        # Build test message to completely fill buffer.
        test_data_prefix = str(MessageCommand.BANDWIDTH_TEST_DATA)
        test_data = (
            test_data_prefix + "\x00" * (buffer_size - len(test_data_prefix))
        )
        test_data = test_data.encode()

        # Record start time and send all the messages.
        time1 = time()
        self._bandwidth_testing_status = BandwidthTestingStatus.SENDING
        for i in range(data_chunks_to_send):
            self._comm_link.send(test_data)

        # Go into waiting state and wait for confirmation from on-board.
        self._bandwidth_testing_status = \
            BandwidthTestingStatus.AWAITING_CONFIRMATION
        self._comm_link.send(
            str(MessageCommand.BANDWIDTH_TEST_REQUEST_CONFIRM).encode()
        )

        while (
            self._bandwidth_testing_status ==
            BandwidthTestingStatus.AWAITING_CONFIRMATION
        ):
            continue
        time2 = time()

        bandwidth = (
            float(data_chunks_to_send * buffer_size) / ((time2 - time1) * 1000)
        )

        print('Completed throughput test...')
        print('Throughput was {:.3f} KB/s'.format(bandwidth))

        # Reset after bandwidth test is complete.
        self._bandwidth_testing_status = None
        self._received_bandwidth_testing_data_count = 0

    # ************** PROCESS INCOMING MESSAGES FROM ONBOARD **************

    def handle_incoming_message(self, msg: bytes) -> None:
        if msg.startswith(b'bandwidth-test-confirm'):
            if (
                self._bandwidth_testing_status ==
                BandwidthTestingStatus.AWAITING_CONFIRMATION
            ):
                self._bandwidth_testing_status = \
                    BandwidthTestingStatus.CONFIRMED
        elif msg.startswith(b'stream-bike-sensor-data'):
            msg_type, *parts = msg.split(b':')
            for i in range(0, len(parts), 3):
                self._record_received_sensor_data(
                    parts[i].decode(),
                    parts[i + 1],
                    parts[i + 2]
                )
        else:
            pass  # Just throw away unknown messages.

    def _record_received_sensor_data(
        self,
        data_type: str,
        data_value: bytes,
        timestamp: bytes
    ) -> None:
        if data_type not in self.received_data[self._SENSOR_DATA_KEY]:
            self.received_data[self._SENSOR_DATA_KEY][data_type] = []

        self.received_data[self._SENSOR_DATA_KEY][data_type].append(
            (timestamp, data_value)
        )

    # ************** SERVICE REQUESTS FOR DATA **************

    def get_recorded_sensor_data(self, data_type: SensorDataKey) -> []:
        if str(data_type) in self.received_data[self._SENSOR_DATA_KEY]:
            return self.received_data[self._SENSOR_DATA_KEY][str(data_type)]
        else:
            return []

    def get_latest_sensor_data(self, data_type: SensorDataKey) -> ():
        if str(data_type) in self.received_data[self._SENSOR_DATA_KEY]:
            return (
                self.received_data[self._SENSOR_DATA_KEY][str(data_type)][-1]
            )
        else:
            return ()


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
                incoming_msg: bytes = self._comm_link.receive()
                self._on_board.handle_incoming_message(incoming_msg)

    def stop(self):
        self._running = False
