import unittest
from unittest.mock import (
    patch,
    MagicMock
)

from client.communication.messages import MessageCommand
from client.communication.on_board import OnBoard


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
