import unittest
from unittest.mock import (
    patch,
    MagicMock
)

from client.communication.messages import MessageCommand
from client.communication.on_board import OnBoard, IncomingMessageHandler


class OnBoardTest(unittest.TestCase):
    """
    A suite of tests surrounding the OnBoard class functionality.
    """

    def setUp(self) -> None:
        self._mock_message_handler = patch(
            'client.communication.on_board.IncomingMessageHandler'
        ).start()
        self.addCleanup(patch.stopall)

    @patch('client.communication.on_board.build_message')
    def test_start_streaming_data(self, mock_build_message):
        """
        Test the start streaming data send message.
        """
        mock_message = 'mock_message'
        mock_build_message.return_value = mock_message
        mock_comm_link = MagicMock()
        mock_comm_link.send = MagicMock()
        on_board = OnBoard(mock_comm_link)
        on_board.start_streaming_data(['key1', 'key2', 'key3'])
        mock_build_message.assert_called_with(
            MessageCommand.STREAM_BIKE_SENSOR_DATA,
            ['start', 'key1', 'key2', 'key3']
        )
        mock_comm_link.send.assert_called_with(mock_message.encode())

    @patch('client.communication.on_board.build_message')
    def test_stop_streaming_data(self, mock_build_message):
        """
        Test the start streaming data send message.
        """
        mock_message = 'mock_message'
        mock_build_message.return_value = mock_message
        mock_comm_link = MagicMock()
        mock_comm_link.send = MagicMock()
        on_board = OnBoard(mock_comm_link)
        on_board.stop_streaming_data(['key1', 'key2', 'key3'])
        mock_build_message.assert_called_with(
            MessageCommand.STREAM_BIKE_SENSOR_DATA,
            ['stop', 'key1', 'key2', 'key3']
        )
        mock_comm_link.send.assert_called_with(mock_message.encode())

    def test_incoming_stream_bike_sensor_data_msg(self):
        """
        Test incoming data from the bike is dealt with properly.
        """

        # Create some test sensor data messages.
        test_sensor_data_messages = [
            b'stream-bike-sensor-data:AIR_TEMPERATURE:30:TIMESTAMP1',
            b'stream-bike-sensor-data:AIR_TEMPERATURE:20:TIMESTAMP2',
            (
                b'stream-bike-sensor-data:'
                b'AIR_TEMPERATURE:10:TIMESTAMP3:'
                b'TYRE_PRESSURE_REAR:5:TIMESTAMP4:'
                b'BRAKE_FRONT_ACTIVE:50:TIMESTAMP5'
            ),
            b'stream-bike-sensor-data:AIR_TEMPERATURE:0:TIMESTAMP6',
        ]

        # Create OnBoard object.
        mock_comm_link = MagicMock()
        on_board = OnBoard(mock_comm_link)

        # Simulate handle_incoming_message call for the test messages.
        for message in test_sensor_data_messages:
            on_board.handle_incoming_message(message)

        # Verify all messages are handled correctly.
        expected_data = {
            'AIR_TEMPERATURE': [
                (b'30', b'TIMESTAMP1'),
                (b'20', b'TIMESTAMP2'),
                (b'10', b'TIMESTAMP3'),
                (b'0', b'TIMESTAMP6')
            ],
            'TYRE_PRESSURE_REAR': [
                (b'5', b'TIMESTAMP4')
            ],
            'BRAKE_FRONT_ACTIVE': [
                (b'50', b'TIMESTAMP5')
            ],
        }

        self.assertListEqual(
            expected_data['AIR_TEMPERATURE'],
            on_board.get_recorded_sensor_data('AIR_TEMPERATURE')
        )
        self.assertListEqual(
            expected_data['TYRE_PRESSURE_REAR'],
            on_board.get_recorded_sensor_data('TYRE_PRESSURE_REAR')
        )
        self.assertListEqual(
            expected_data['BRAKE_FRONT_ACTIVE'],
            on_board.get_recorded_sensor_data('BRAKE_FRONT_ACTIVE')
        )


class IncomingMessageHandlerTest(unittest.TestCase):
    """
    A suite of tests surrounding the IncomingMessageHandler class
    functionality.
    """

    def test_incoming_msg_handled(self):
        """
        Test incoming message is handled correctly.
        """
        test_msg = 'test:message'

        mock_comm_link = MagicMock()
        mock_comm_link.receive.side_effect = [test_msg]
        mock_on_board = MagicMock()

        msg_handler = IncomingMessageHandler(mock_on_board, mock_comm_link)

        # Run the thread in a try catch and ignore StopIteration errors.  It
        # will error because it will exhaust the side_effect list and cause
        # a StopIteration error. This is a messy solution but it works to
        # test for now.
        try:
            msg_handler.run()
        except StopIteration:
            pass

        mock_on_board.handle_incoming_message.assert_called_with(test_msg)
