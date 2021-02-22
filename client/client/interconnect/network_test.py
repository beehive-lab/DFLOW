import socket
import unittest
from unittest.mock import patch, MagicMock, call

from client.interconnect.network import NetworkLink


class NetworkLinkTestCase(unittest.TestCase):
    """
    A suite of tests surrounding the NetworkLink class.
    """

    def setUp(self) -> None:
        self._test_host_name = 'test_host'
        self._test_port_num = 8080
        self._test_data = b'some_test_binary_string'

    @patch('socket.socket')
    def test_network_link_create(self, mock_socket_constructor):
        """
        Test that network link is created correctly.
        """

        # Create a mock socket instance to be returned by
        # mock_socket_constructor.
        mock_socket_instance = MagicMock()

        # Set return value of the mock socket constructor to
        # mock_socket_instance.
        mock_socket_constructor.return_value = mock_socket_instance

        # Create the network link.
        NetworkLink(
            self._test_host_name,
            self._test_port_num
        )

        # Make sure the socket was created with correct parameters.
        mock_socket_constructor.assert_called_with(
            socket.AF_INET,
            socket.SOCK_STREAM
        )

    @patch('socket.socket')
    def test_network_link_connect(self, mock_socket_constructor):
        """
        Test that network link connects correctly.
        """

        # Create a mock socket instance to be returned by
        # mock_socket_constructor.
        mock_socket_instance = MagicMock()

        # Set return value of the mock socket constructor to
        # mock_socket_instance.
        mock_socket_constructor.return_value = mock_socket_instance

        # Create the network link and call connect().
        network_link = NetworkLink(
            self._test_host_name,
            self._test_port_num
        )
        network_link.connect()

        # Make sure connect was called on the socket with correct
        # parameters.
        mock_socket_instance.connect.assert_called_with(
            (self._test_host_name, self._test_port_num)
        )

    @patch('socket.socket')
    def test_network_link_disconnect(self, mock_socket_constructor):
        """
        Test that network link disconnects correctly.
        """

        # Create a mock socket instance to be returned by
        # mock_socket_constructor.
        mock_socket_instance = MagicMock()

        # Set return value of the mock socket constructor to
        # mock_socket_instance.
        mock_socket_constructor.return_value = mock_socket_instance

        # Create the network link and call connect() then disconnect.
        network_link = NetworkLink(
            self._test_host_name,
            self._test_port_num
        )
        network_link.connect()
        network_link.disconnect()

        # Make sure disconnect was called on the socket.
        mock_socket_instance.close.assert_called()

    @patch('socket.socket')
    def test_network_link_reconnect(self, mock_socket_constructor):
        """
        Test that network link reconnects correctly.
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

        # Create the network link and call connect() then disconnect.
        network_link = NetworkLink(
            self._test_host_name,
            self._test_port_num
        )
        network_link.reconnect()

        # Check that when reconnect is called the existing socket is first
        # closed before a new one is created.
        self.assertEqual(
            [
                call.create(socket.AF_INET, socket.SOCK_STREAM),
                call.close(),
                call.create(socket.AF_INET, socket.SOCK_STREAM),
                call.connect((self._test_host_name, self._test_port_num))
            ],
            mock_call_manager.mock_calls
        )

    @patch('socket.socket')
    def test_network_link_send(self, mock_socket_constructor):
        """
        Test that network link sends data correctly.
        """

        # Create a mock socket instance to be returned by
        # mock_socket_constructor.
        mock_socket_instance = MagicMock()

        # Set return value of the mock socket constructor to
        # mock_socket_instance.
        mock_socket_constructor.return_value = mock_socket_instance

        # Create the network link.
        network_link = NetworkLink(
            self._test_host_name,
            self._test_port_num
        )

        # Create a byte string and attempt to send it through the network link.
        network_link.send(self._test_data)

        # Make sure the correct data was passed to the underlying socket.
        mock_socket_instance.sendall.assert_called_with(self._test_data)

    @patch('socket.socket')
    def test_network_link_receive(self, mock_socket_constructor):
        """
        Test that network link receives data correctly.
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

        # Create the network link.
        network_link = NetworkLink(
            self._test_host_name,
            self._test_port_num
        )

        # Call the receive method on the network link and store the data
        # returned.
        actual_data = network_link.receive()

        # Make sure the recv() method of the underlying socket was called
        # and the data was returned as expected.
        mock_socket_instance.recv.assert_called()
        self.assertEqual(expected_data, actual_data)


if __name__ == '__main__':
    unittest.main()
