"""
The main entrypoint for client.
"""
import os
from dataclasses import dataclass
from enum import Enum

import yaml

from client.communication.on_board import OnBoard, IncomingMessageHandler
from client.interconnect.bluetooth import BluetoothLink
from client.interconnect.commlink import CommLink
from client.interconnect.network import NetworkLink


@dataclass
class AppConfig:
    """
    Contains information about the applications configuration.
    """
    cert_file_path: str
    priv_key_file_path: str
    ca_cert_file_path: str
    help_dict: {str: str}
    socket_timeout: int


class ConnectionMethod(Enum):
    BLUETOOTH = 1
    NETWORK = 2


class Client:

    def __init__(self, app_config: AppConfig):
        self._app_config = app_config

    def start(self):
        print('Client started...')

        with self.get_connection_to_on_board() as comm_link:

            on_board: OnBoard = OnBoard(comm_link)
            comm_link.connect()

            msg_handler: IncomingMessageHandler = IncomingMessageHandler(
                on_board
            )

            try:
                msg_handler.start()

                print('Successfully connected to on-board...')

                self.print_menu()

                next_command: str
                while True:
                    next_command = input('command: ').strip()

                    if next_command.lower().strip() == 'exit':
                        break

                    self.process_command(next_command, on_board, msg_handler)
            finally:
                print('Preparing to shut down...')
                msg_handler.stop()
                msg_handler.join()
                print('Shutting down...')

    def get_connection_to_on_board(self) -> CommLink:
        """
        Get a CommLink to to use to communicate with the on-board.
        """

        comm_link: CommLink

        connection_method: ConnectionMethod = \
            self.get_preferred_connection_method_from_user()

        use_secure_connection: bool = \
            self.get_use_secure_connection_from_user()

        if connection_method == ConnectionMethod.BLUETOOTH:
            mac = input('Enter on-board bluetooth MAC address: ')
            channel = int(input('Enter on-board bluetooth channel: '))
            comm_link = BluetoothLink(
                mac,
                channel,
                self._app_config.ca_cert_file_path,
                self._app_config.cert_file_path,
                self._app_config.priv_key_file_path,
                secure=use_secure_connection,
                timeout=self._app_config.socket_timeout
            )
        else:
            host = input('Enter on-board hostname: ')
            port = int(input('Enter on-board port: '))
            comm_link = NetworkLink(
                host,
                port,
                self._app_config.ca_cert_file_path,
                self._app_config.cert_file_path,
                self._app_config.priv_key_file_path,
                secure=use_secure_connection,
                timeout=self._app_config.socket_timeout
            )

        return comm_link

    def get_preferred_connection_method_from_user(self) -> ConnectionMethod:
        """
        Prompt the user to select preferred connection method.
        Default to NETWORK.
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

    def get_use_secure_connection_from_user(self) -> bool:
        """
        Prompt the user to select whether or not they wish to connect to the
        on-board using a secure connection.
        :return: true if yes false if no
        """
        use_secure_conn = input(
            'Do you wish to use a secure connection (TLS) ' +
            'to communicate with on-board? (y/n):'
        )

        if use_secure_conn.lower() == 'n':
            return False
        else:
            return True

    def print_menu(self):
        print(
            'Choose from the following commands:\n'
            '   - exit\n'
            '   - menu\n'
            '   - help:<command_name>\n'
            '   - start_sensor_data_stream:<data_key_1>:...:<data_key_n>\n'
            '   - stop_sensor_data_stream:<data_key_1>:...:<data_key_n>\n'
            '   - print_recorded_sensor_data:<data_key>'
        )

    def process_command(
        self,
        command: str,
        on_board,
        msg_handler
    ) -> None:
        command, *args = command.strip().split(':')
        command = command.lower().strip()

        if command == 'help':
            if not args:
                self.print_help('default')
            else:
                self.print_help(args[0])
        elif command == 'menu':
            self.print_menu()
        elif command == 'start_sensor_data_stream':
            on_board.start_streaming_data(args)
        elif command == 'stop_sensor_data_stream':
            on_board.stop_streaming_data(args)
        elif command == 'print_recorded_sensor_data':
            if not args:
                print(
                    'Error no argument supplied try '
                    '\'help print_recorded_sensor_data\' for help'
                )
            print('*****************************************')
            print('Recorded data for: ', args[0])
            print(msg_handler.get_recorded_sensor_data(args[0]))
            print('*****************************************')
        else:
            print(
                '\'{}\' is not a valid command. Type \'menu\' for a list of '
                'available commands'.format(command)
            )

    def print_help(self, arg: str):
        if not arg:
            print(self._app_config.help_dict['default'])
        elif arg in self._app_config.help_dict:
            print(self._app_config.help_dict[arg])
        else:
            print('No help info available for \'{}\''.format(arg))


def load_app_config() -> AppConfig:
    app_config: AppConfig

    with open('config.yml', 'r') as config_file:
        config = yaml.load(config_file, Loader=yaml.FullLoader)
        app_config = AppConfig(
            os.path.expanduser(config['security']['cert_file_path']),
            os.path.expanduser(config['security']['priv_key_file_path']),
            os.path.expanduser(config['security']['ca_cert_file_path']),
            config['help'],
            config['socket']['timeout']
        )

    return app_config


def main() -> None:
    app_config: AppConfig = load_app_config()
    client: Client = Client(app_config)
    client.start()


if __name__ == "__main__":
    main()
