from threading import Thread, Event
from time import time

from client.communication.messages import (
    SensorDataKey,
    MessageCommand,
    build_command_message_with_args,
    ProfilingDataKey
)
from client.interconnect.commlink import CommLink
from client.interconnect.network import NetworkLink


class OnBoard:
    """
    Represents the on-board module.
    """

    _SENSOR_DATA_KEY = 'sensor_data'
    _PROFILING_DATA_KEY = 'profiling_data'

    def __init__(self, comm_link: CommLink):
        # The communications link to the on-board.
        self._comm_link: CommLink = comm_link
        self._comm_link.connect()

        # Variables used for bandwidth testing.
        self._wait_for_bandwidth_test_confirmation: Event = Event()
        self._wait_to_finish_receiving_bandwidth_test_data: Event = Event()
        self._wait_for_result_of_bandwidth_test: Event = Event()
        self._result_of_bandwidth_test: float = None

        # Variables used for performing sort benchmark.
        self._wait_for_sort_benchmark_result: Event = Event()
        self._result_of_sorting_benchmarking: float = None

        # Data received from the on-board.
        self.received_data: dict[str, dict[str, list[tuple[bytes, bytes]]]] = \
            dict()
        self.received_data[self._SENSOR_DATA_KEY] = dict()
        self.received_data[self._PROFILING_DATA_KEY] = dict()

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

    def start_streaming_sensor_data(self, data_keys: [SensorDataKey]) -> None:
        msg_command: MessageCommand = MessageCommand.STREAM_BIKE_SENSOR_DATA
        msg_args: [str] = ['start'] + [str(data_key) for data_key in data_keys]

        msg: str = build_command_message_with_args(
            msg_command,
            msg_args
        )

        self._comm_link.send(msg.encode())

    def stop_streaming_sensor_data(self, data_keys: [SensorDataKey]) -> None:
        msg_command: MessageCommand = MessageCommand.STREAM_BIKE_SENSOR_DATA
        msg_args: [str] = ['stop'] + [str(data_key) for data_key in data_keys]

        message = build_command_message_with_args(
            msg_command,
            msg_args
        )

        self._comm_link.send(message.encode())

    def start_streaming_profiling_data(
        self,
        data_keys: [SensorDataKey]
    ) -> None:
        msg_command: MessageCommand = MessageCommand.STREAM_PROFILING_DATA
        msg_args: [str] = ['start'] + [str(data_key) for data_key in data_keys]

        message: str = build_command_message_with_args(
            msg_command,
            msg_args
        )

        self._comm_link.send(message.encode())

    def stop_streaming_profiling_data(
        self,
        data_keys: [SensorDataKey]
    ) -> None:
        msg_command: MessageCommand = MessageCommand.STREAM_PROFILING_DATA
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
        print('measuring throughput from client to on-board...')

        # Reset all the variables for bandwidth testing.
        self._wait_for_bandwidth_test_confirmation.clear()
        self._wait_to_finish_receiving_bandwidth_test_data.clear()
        self._wait_for_result_of_bandwidth_test.clear()
        self._result_of_bandwidth_test = None

        # Signal start of bandwidth test to on-board.
        self._comm_link.send(
            str(MessageCommand.BANDWIDTH_TEST_START).encode()
        )

        # Set size and number of messages to be sent.
        chunk_size = 1024
        data_chunks_to_send: int
        if isinstance(self._comm_link, NetworkLink):
            data_chunks_to_send = 100000
        else:
            data_chunks_to_send = 1000

        # Build test message to completely fill buffer.
        test_data_prefix = str(MessageCommand.BANDWIDTH_TEST_DATA)
        test_data = (
            test_data_prefix + "\x00" * (chunk_size - len(test_data_prefix))
        )
        test_data = test_data.encode()

        # Record start time and send all the messages.
        time1 = time()
        for i in range(data_chunks_to_send):
            self._comm_link.send(test_data)

        # Request confirmation from on-board and wait.
        self._comm_link.send(
            str(MessageCommand.BANDWIDTH_TEST_REQUEST_CONFIRM).encode()
        )
        self._wait_for_bandwidth_test_confirmation.wait()
        time2 = time()

        bandwidth_to_on_board = (
            float(data_chunks_to_send * chunk_size) / ((time2 - time1) * 1000)
        )

        # The on-board will now do the same test so we wait until it
        # requests confirmation.
        print('measuring throughput from on-board to client...')
        self._wait_to_finish_receiving_bandwidth_test_data.wait()

        # Confirm data was received.
        self._comm_link.send(
            str(MessageCommand.BANDWIDTH_TEST_CONFIRM).encode()
        )

        # After confirmation wait for on-board to share result.
        self._wait_for_result_of_bandwidth_test.wait()
        bandwidth_from_on_board = self._result_of_bandwidth_test

        print('Completed throughput test...')
        print(
            'Throughput:\n'
            '   client->on-board: {:.3f} KB/s\n'
            '   on-board->client: {:.3f} KB/s'.format(
                bandwidth_to_on_board,
                bandwidth_from_on_board
            )
        )

        # Reset all the variables for bandwidth testing.
        self._wait_for_bandwidth_test_confirmation.clear()
        self._wait_to_finish_receiving_bandwidth_test_data.clear()
        self._wait_for_result_of_bandwidth_test.clear()
        self._result_of_bandwidth_test = None

    def perform_sort_benchmark(self, number_of_items_to_sort: int):
        # Reset all the variables for sort benchmarking.
        self._wait_for_sort_benchmark_result.clear()
        self._result_of_sorting_benchmarking = None

        # Send sort benchmark message.
        msg: str = build_command_message_with_args(
            MessageCommand.SORT_BENCHMARK,
            [number_of_items_to_sort]
        )
        self._comm_link.send(msg.encode())

        print('Waiting for on-board to complete sorting benchmark...')

        # Wait for sort benchmark result.
        self._wait_for_sort_benchmark_result.wait()
        sort_benchmark_result = self._result_of_sorting_benchmarking

        print(
            'Sort benchmark complete:\n'
            '   - # of items sorted: {}\n'
            '   - total sorting time: {:.6f}s'.format(
                number_of_items_to_sort,
                sort_benchmark_result
            )
        )

        # Reset all the variables for sort benchmarking.
        self._wait_for_sort_benchmark_result.clear()
        self._result_of_sorting_benchmarking = None

    # ************** PROCESS INCOMING MESSAGES FROM ONBOARD **************

    def handle_incoming_message(self, msg: bytes) -> None:
        if msg.startswith(b'bandwidth-test-confirm'):
            self._wait_for_bandwidth_test_confirmation.set()
        elif msg.startswith(b'bandwidth-test-request-confirm'):
            self._wait_to_finish_receiving_bandwidth_test_data.set()
        elif msg.startswith(b'bandwidth-test-result'):
            self._result_of_bandwidth_test = float(msg.split(b':')[1])
            self._wait_for_result_of_bandwidth_test.set()
        elif msg.startswith(b'sort-benchmark-result'):
            self._result_of_sorting_benchmarking = float(msg.split(b':')[1])
            self._wait_for_sort_benchmark_result.set()
        elif msg.startswith(b'stream-bike-sensor-data'):
            msg_type, *parts = msg.split(b':')
            for i in range(0, len(parts), 3):
                self._record_received_sensor_data(
                    parts[i].decode(),
                    parts[i + 1],
                    parts[i + 2]
                )
        elif msg.startswith(b'stream-profiling-data'):
            msg_type, *parts = msg.split(b':')
            for i in range(0, len(parts), 3):
                self._record_received_profiling_data(
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

    def _record_received_profiling_data(
        self,
        data_type: str,
        data_value: bytes,
        timestamp: bytes
    ) -> None:
        if data_type not in self.received_data[self._PROFILING_DATA_KEY]:
            self.received_data[self._PROFILING_DATA_KEY][data_type] = []
        self.received_data[self._PROFILING_DATA_KEY][data_type].append(
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

    def get_recorded_profiling_data(self, data_type: ProfilingDataKey) -> []:
        if str(data_type) in self.received_data[self._PROFILING_DATA_KEY]:
            return self.received_data[self._PROFILING_DATA_KEY][str(data_type)]
        else:
            return []

    def get_latest_profiling_data(self, data_type: ProfilingDataKey) -> ():
        if str(data_type) in self.received_data[self._PROFILING_DATA_KEY]:
            return (
                self.received_data[self._PROFILING_DATA_KEY][str(data_type)][
                    -1
                ]
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
