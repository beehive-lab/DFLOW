"""
The main entrypoint for client.
"""
import os
from dataclasses import dataclass
from enum import Enum

import yaml

from client.communication.on_board import OnBoard
from client.interconnect.bluetooth import BluetoothLink
from client.interconnect.commlink import CommLink
from client.interconnect.network import NetworkLink


class ConnectionMethod(Enum):
    BLUETOOTH = 1
    NETWORK = 2


def get_preferred_connection_method_from_user() -> ConnectionMethod:
    """
    Prompt the user to select preferred connection method. Default to NETWORK.
    :return: the ConnectionMethod selected by the user.
    """

    preferred: str = input(
        'Choose a method of connecting to on-board:\n' +
        '"n" - regular network connection\n' +
        '"b" - bluetooth\n' +
        'Enter choice: '
    )

    if preferred.lower() == 'b':
        return ConnectionMethod.BLUETOOTH
    else:
        return ConnectionMethod.NETWORK


def get_connection_to_on_board() -> CommLink:
    """
    Get a CommLink to to use to communicate with the on-board.
    """

    comm_link: CommLink

    connection_method: ConnectionMethod = \
        get_preferred_connection_method_from_user()

    if connection_method == ConnectionMethod.BLUETOOTH:
        mac = input('Enter on-board bluetooth MAC address: ')
        channel = int(input('Enter on-board bluetooth channel: '))
        comm_link = BluetoothLink(
            mac,
            channel,
            get_app_config().ca_cert_file_path,
            get_app_config().cert_file_path,
            get_app_config().priv_key_file_path
        )
    else:
        host = input('Enter on-board hostname: ')
        port = int(input('Enter on-board port: '))
        comm_link = NetworkLink(
            host,
            port,
            get_app_config().ca_cert_file_path,
            get_app_config().cert_file_path,
            get_app_config().priv_key_file_path
        )

    return comm_link


@dataclass
class AppConfig:
    """
    Contains information about the applications configuration.
    """
    cert_file_path: str
    priv_key_file_path: str
    ca_cert_file_path: str
    help_dict: {str: str}


app_config: AppConfig


def load_app_config() -> None:
    global app_config
    with open('config.yml', 'r') as config_file:
        config = yaml.load(config_file, Loader=yaml.FullLoader)
        app_config = AppConfig(
            os.path.expanduser(config['security']['cert_file_path']),
            os.path.expanduser(config['security']['priv_key_file_path']),
            os.path.expanduser(config['security']['ca_cert_file_path']),
            config['help']
        )


def get_app_config() -> AppConfig:
    global app_config
    return app_config


def print_menu():
    print(
        'Choose from the following commands:\n'
        '   - exit\n'
        '   - menu\n'
        '   - help:<command_name>\n'
        '   - start_sensor_data_stream:<data_key_1>:...:<data_key_n>\n'
        '   - stop_sensor_data_stream:<data_key_1>:...:<data_key_n>'
    )


def print_help(arg: str):
    if not arg:
        print(get_app_config().help_dict['default'])
    elif arg in get_app_config().help_dict:
        print(get_app_config().help_dict[arg])
    else:
        print('No help info available for \'{}\''.format(arg))


def process_command(command: str, on_board: OnBoard) -> None:
    command, *args = command.strip().split(':')
    command = command.lower().strip()

    if command == 'help':
        if not args:
            print_help('default')
        else:
            print_help(args[0])
    elif command == 'menu':
        print_menu()
    elif command == 'start_sensor_data_stream':
        on_board.start_streaming_data(args)
    elif command == 'stop_sensor_data_stream':
        on_board.stop_streaming_data(args)
    else:
        print(
            '\'{}\' is not a valid command. Type \'menu\' for a list of '
            'available commands'.format(command)
        )


def main() -> None:
    print('Client started...')

    load_app_config()

    with get_connection_to_on_board() as comm_link:
        comm_link.connect()
        on_board: OnBoard = OnBoard(comm_link)

        print('Successfully connected to on-board...')

        print_menu()

        next_command: str
        while True:
            next_command = input('command: ').strip()

            if next_command.lower().strip() == 'exit':
                break

            process_command(next_command, on_board)


if __name__ == "__main__":
    main()
