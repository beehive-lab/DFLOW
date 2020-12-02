import socket
import unittest
from unittest.mock import patch, MagicMock, call

from client.interconnect.bluetooth import BluetoothLink


class BluetoothLinkTestCase(unittest.TestCase):
    """
    A suite of tests surrounding the BluetoothLink class.
    """

    def setUp(self) -> None:
        self._test_mac_address = 'A:MAC:ADDRESS'
        self._test_channel_num = 4
        self._test_data = b'some_test_binary_string'

    @patch('socket.socket')
    def test_bluetooth_link_create(self, mock_socket_constructor):
        """
        Test that bluetooth link is created correctly.
        """

        # Create a mock socket instance to be returned by
        # mock_socket_constructor.
        mock_socket_instance = MagicMock()

        # Set return value of the mock socket constructor to
        # mock_socket_instance.
        mock_socket_constructor.return_value = mock_socket_instance

        # Create the bluetooth link.
        BluetoothLink(
            self._test_mac_address,
            self._test_channel_num
        )

        # Make sure the socket was created with correct parameters.
        mock_socket_constructor.assert_called_with(
            socket.AF_BLUETOOTH,
            socket.SOCK_STREAM,
            socket.BTPROTO_RFCOMM
        )

    @patch('socket.socket')
    def test_bluetooth_link_connect(self, mock_socket_constructor):
        """
        Test that bluetooth link connects correctly.
        """

        # Create a mock socket instance to be returned by
        # mock_socket_constructor.
        mock_socket_instance = MagicMock()

        # Set return value of the mock socket constructor to
        # mock_socket_instance.
        mock_socket_constructor.return_value = mock_socket_instance

        # Create the bluetooth link and call connect().
        bluetooth_link = BluetoothLink(
            self._test_mac_address,
            self._test_channel_num
        )
        bluetooth_link.connect()

        # Make sure connect was called on the socket with correct
        # parameters.
        mock_socket_instance.connect.assert_called_with(
            (self._test_mac_address, self._test_channel_num)
        )

    @patch('socket.socket')
    def test_bluetooth_link_disconnect(self, mock_socket_constructor):
        """
        Test that bluetooth link disconnects correctly.
        """

        # Create a mock socket instance to be returned by
        # mock_socket_constructor.
        mock_socket_instance = MagicMock()

        # Set return value of the mock socket constructor to
        # mock_socket_instance.
        mock_socket_constructor.return_value = mock_socket_instance

        # Create the bluetooth link and call connect() then disconnect.
        bluetooth_link = BluetoothLink(
            self._test_mac_address,
            self._test_channel_num
        )
        bluetooth_link.connect()
        bluetooth_link.disconnect()

        # Make sure disconnect was called on the socket.
        mock_socket_instance.close.assert_called()

    @patch('socket.socket')
    def test_bluetooth_link_reconnect(self, mock_socket_constructor):
        """
        Test that bluetooth link reconnects correctly.
        """

        # Create a mock socket instance to be returned by
        # mock_socket_constructor.
        mock_socket_instance = MagicMock()

        # Set return value of the mock socket constructor to
        # mock_socket_instance.
        mock_socket_constructor.return_value = mock_socket_instance

        # Create a manager mock that we can use to verify the order of
        # calls is correct.
        mock_call_manager = MagicMock()
        mock_call_manager.attach_mock(
            mock_socket_constructor,
            'create'
        )
        mock_call_manager.attach_mock(mock_socket_instance.connect, 'connect')
        mock_call_manager.attach_mock(mock_socket_instance.close, 'close')

        # Create the bluetooth link and call connect() then disconnect.
        bluetooth_link = BluetoothLink(
            self._test_mac_address,
            self._test_channel_num
        )
        bluetooth_link.reconnect()

        # Check that when reconnect is called the existing socket is first
        # closed before a new one is created.
        self.assertEqual(
            [
                call.create(
                    socket.AF_BLUETOOTH,
                    socket.SOCK_STREAM,
                    socket.BTPROTO_RFCOMM
                ),
                call.close(),
                call.create(
                    socket.AF_BLUETOOTH,
                    socket.SOCK_STREAM,
                    socket.BTPROTO_RFCOMM
                ),
                call.connect((self._test_mac_address, self._test_channel_num))
            ],
            mock_call_manager.mock_calls
        )

    @patch('socket.socket')
    def test_bluetooth_link_send(self, mock_socket_constructor):
        """
        Test that bluetooth link sends data correctly.
        """

        # Create a mock socket instance to be returned by
        # mock_socket_constructor.
        mock_socket_instance = MagicMock()

        # Set return value of the mock socket constructor to
        # mock_socket_instance.
        mock_socket_constructor.return_value = mock_socket_instance

        # Create the bluetooth link.
        bluetooth_link = BluetoothLink(
            self._test_mac_address,
            self._test_channel_num
        )

        # Create a byte string and attempt to send it through the bluetooth
        # link.
        bluetooth_link.send(self._test_data)

        # Make sure the correct data was passed to the underlying socket.
        mock_socket_instance.sendall.assert_called_with(self._test_data)

    @patch('socket.socket')
    def test_bluetooth_link_receive(self, mock_socket_constructor):
        """
        Test that bluetooth link receives data correctly.
        """

        # Create a mock socket instance to be returned by
        # mock_socket_constructor.
        mock_socket_instance = MagicMock()

        # Set return value of the mock socket constructor to
        # mock_socket_instance.
        mock_socket_constructor.return_value = mock_socket_instance

        # Create some data and set it as the return value for the recv
        # method of the underlying socket.
        expected_data = self._test_data
        mock_socket_instance.recv.return_value = expected_data

        # Create the bluetooth link.
        bluetooth_link = BluetoothLink(
            self._test_mac_address,
            self._test_channel_num
        )

        # Call the receive method on the bluetooth link and store the data
        # returned.
        actual_data = bluetooth_link.receive()

        # Make sure the recv() method of the underlying socket was called
        # and the data was returned as expected.
        mock_socket_instance.recv.assert_called()
        self.assertEqual(expected_data, actual_data)


if __name__ == '__main__':
    unittest.main()
