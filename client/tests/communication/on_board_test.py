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
            ['key1', 'key2', 'key3', 'start']
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
            ['key1', 'key2', 'key3', 'stop']
        )
        mock_comm_link.send.assert_called_with(mock_message.encode())


class IncomingMessageHandlerTest(unittest.TestCase):
    """
    A suite of tests surrounding the IncomingMessageHandler class
    functionality.
    """

    def test_incoming_stream_bike_sensor_data_msg(self):
        """
        Test incoming data from the bike is dealt with properly.
        """

        # Set up  a mock onboard object with some dummy incoming messages
        # to be dealt with by the message handler.
        mock_on_board = MagicMock()
        mock_on_board.is_connected.return_value = True
        mock_on_board.get_incoming_msg = MagicMock()
        mock_on_board.get_incoming_msg.side_effect = [
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

        # Set up  the message handler object.
        msg_handler = IncomingMessageHandler(mock_on_board)

        # Run the thread in a try catch and ignore StopIteration errors.  It
        # will error because it will exhaust the side_effect list and cause
        # a StopIteration error. This is a messy solution but it works to
        # test for now.
        try:
            msg_handler.run()
        except StopIteration:
            pass

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
            msg_handler.get_recorded_sensor_data('AIR_TEMPERATURE')
        )
        self.assertListEqual(
            expected_data['TYRE_PRESSURE_REAR'],
            msg_handler.get_recorded_sensor_data('TYRE_PRESSURE_REAR')
        )
        self.assertListEqual(
            expected_data['BRAKE_FRONT_ACTIVE'],
            msg_handler.get_recorded_sensor_data('BRAKE_FRONT_ACTIVE')
        )
