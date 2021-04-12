import unittest

from parameterized import parameterized

from client.communication.messages import (
    build_message,
    MessageCommand,
    SensorDataKey
)


class MessagesTest(unittest.TestCase):
    """
    A suite of tests surrounding the messages utility module functionality.
    """

    @parameterized.expand([
        [
            MessageCommand.STREAM_BIKE_SENSOR_DATA,
            [],
            'stream-bike-sensor-data'
        ],
        ['test', ['arg1', 'arg2', 'arg3'], 'test:arg1:arg2:arg3'],
        [
            MessageCommand.STREAM_BIKE_SENSOR_DATA,
            [
                SensorDataKey.AIR_TEMPERATURE,
                SensorDataKey.THROTTLE_POSITION,
                'test'
            ],
            'stream-bike-sensor-data:AIR_TEMPERATURE:THROTTLE_POSITION:test'
        ]
    ])
    def test_build_message(self, command, args, expected_message):
        """
        Test messages are built correctly.
        """
        self.assertEqual(
            expected_message,
            build_message(command, args)
        )
