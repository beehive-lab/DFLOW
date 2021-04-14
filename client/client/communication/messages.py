from enum import Enum


class MessageCommand(Enum):
    """
    The different commands that can be given to the on-board.
    """
    STREAM_BIKE_SENSOR_DATA = 'stream-bike-sensor-data'
    ENCRYPTION = 'encryption'
    BANDWIDTH_TEST_START = 'start-bandwidth-test'
    BANDWIDTH_TEST_REQUEST_CONFIRM = 'bandwidth-test-request-confirm'
    BANDWIDTH_TEST_DATA = 'bandwidth-test-data'
    BANDWIDTH_TEST_CONFIRM = 'bandwidth-test-confirm'

    def __str__(self):
        return self.value


class SensorDataKey(Enum):
    """
    The keys for different types of bike sensor data.
    """
    AIR_TEMPERATURE = 'AIR_TEMPERATURE'
    THROTTLE_POSITION = 'THROTTLE_POSITION'
    TYRE_PRESSURE_FRONT = 'TYRE_PRESSURE_FRONT'
    TYRE_PRESSURE_REAR = 'TYRE_PRESSURE_REAR'
    MOTORCYCLE_SPEED = "MOTORCYCLE_SPEED"
    REAR_WHEEL_SPEED = 'REAR_WHEEL_SPEED'
    FRONT_WHEEL_SPEED = 'FRONT_WHEEL_SPEED'
    BRAKE_REAR_ACTIVE = 'BRAKE_REAR_ACTIVE'
    BRAKE_FRONT_ACTIVE = 'BRAKE_FRONT_ACTIVE'
    ABS_MODE = 'ABS_MODE'
    TC_MODE = 'TC_MODE'
    THROTTLE_RESPONSE_MODE = 'THROTTLE_RESPONSE_MODE'
    LEAN_ANGLE = 'LEAN_ANGLE'
    BATTERY_VOLTAGE = 'BATTERY_VOLTAGE'
    OIL_PRESSURE = 'OIL_PRESSURE'
    GEAR_POSITION = 'GEAR_POSITION'
    WATER_TEMPERATURE = 'WATER_TEMPERATURE'
    ENGINE_SPEED = 'ENGINE_SPEED'
    TIMESTAMP = 'TIMESTAMP'

    def __str__(self):
        return self.value


def build_command_message_with_args(
    command: MessageCommand,
    args: [str]
) -> str:
    """
    Builds a message to send to the on-board.

    :param command: the message type.
    :param args: the arguments to the command.
    :return: The build message string to send.
    """
    return ':'.join(([str(command)] + [str(arg) for arg in args]))
