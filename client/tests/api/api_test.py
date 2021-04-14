import json
import unittest
from unittest.mock import (
    patch, MagicMock, ANY
)

from client.api.api import start_api_server, api_root, sensor_data_keys, \
    live_sensor_data, all_live_sensor_data
from client.communication.messages import SensorDataKey


class ApiTest(unittest.TestCase):
    """
    A suite of tests surrounding the api functionality.
    """

    def setUp(self) -> None:
        pass

    @patch('client.api.api.ServerThread')
    def test_start_api(self, mock_server_thread_constructor):
        """
        Make sure API is started correctly.
        """

        mock_server_thread = MagicMock()
        mock_server_thread_constructor.return_value = mock_server_thread

        mock_on_board = MagicMock()
        api_host = 'test_host'
        api_port = 5000

        start_api_server(mock_on_board, api_host, api_port)

        mock_server_thread_constructor.assert_called_with(
            ANY,
            api_host,
            api_port
        )
        mock_server_thread.start.assert_called()

    @patch('client.api.api.render_template')
    def test_root(self, mock_render_template):
        """
        Test that the index page is rendered.
        """
        api_root()
        mock_render_template.assert_called_with('index.html')

    @patch('client.api.api.jsonify')
    def test_sensor_data_keys(self, mock_jsonify):
        """
        Test that all sensor data keys are returned correctly.
        """
        mock_jsonify.side_effect = json.dumps
        expected = json.dumps([str(key) for key in SensorDataKey])
        actual = sensor_data_keys()
        self.assertEqual(expected, actual)

    @patch('client.api.api.jsonify')
    @patch('client.api.api.on_board')
    def test_live_sensor_data_exists(self, mock_on_board, mock_jsonify):
        """
        Test that live sensor data for a particular key is returned correctly
        if it exists.
        """
        mock_jsonify.side_effect = json.dumps

        test_key = 'test_key'
        test_timestamp = b'1489256431'
        test_value = b'100'

        mock_on_board.get_latest_sensor_data.return_value = (
            test_timestamp,
            test_value
        )

        expected = json.dumps(
            {
                'key': str(test_key),
                'timestamp': int(test_timestamp),
                'value': float(test_value)
            }
        )

        actual = live_sensor_data(test_key)

        self.assertEqual(expected, actual)

    @patch('client.api.api.jsonify')
    @patch('client.api.api.on_board')
    def test_live_sensor_data_doesnt_exist(self, mock_on_board, mock_jsonify):
        """
        Test that correct result is returned if sensor data for key does not
        exist.
        """
        mock_jsonify.side_effect = json.dumps

        test_key = 'test_key'
        mock_on_board.get_latest_sensor_data.return_value = ()

        expected = json.dumps(
            {
                'key': str(test_key),
                'timestamp': None,
                'value': None
            }
        )

        actual = live_sensor_data(test_key)

        self.assertEqual(expected, actual)

    @patch('client.api.api.jsonify')
    @patch('client.api.api.on_board')
    def test_all_live_sensor_data(self, mock_on_board, mock_jsonify):
        """
        Test that correct result is returned when all live sensor data is
        requested.
        """
        mock_jsonify.side_effect = json.dumps

        test_timestamp = b'1489256431'
        test_value = b'100'
        dummy_data = (test_timestamp, test_value)
        mock_on_board.get_latest_sensor_data.return_value = dummy_data

        expected = json.dumps(
            [
                {
                    'key': str(sensor_data_key),
                    'timestamp': int(dummy_data[0]),
                    'value': float(dummy_data[1])
                } for sensor_data_key in SensorDataKey
            ]
        )

        actual = all_live_sensor_data()

        self.assertEqual(expected, actual)
