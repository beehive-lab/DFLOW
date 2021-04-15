import socket
import unittest
from unittest.mock import (
    patch,
    MagicMock,
    call
)

from client.interconnect.network import NetworkLink


class NetworkLinkTestCase(unittest.TestCase):
    """
    A suite of tests surrounding the NetworkLink class.
    """

    def setUp(self) -> None:
        self._test_host_name = 'test_host'
        self._test_port_num = 8080
        self._test_data = b'some_test_binary_string'
        self._test_ca_cert = 'test_ca_cert'
        self._test_cert = 'test_cert'
        self._test_key = 'test_key'
        self._setup_mocks_and_patches()

    def _setup_mocks_and_patches(self):
        """
        Set up all the patches and mocks needed for the tests in this suite.
        """

        # Set up patches for socket.Socket() create_default_context()
        # along with and add cleanup.
        self._mock_socket_constructor = patch('socket.socket').start()
        self._mock_create_default_context = (
            patch('ssl.create_default_context').start()
        )
        self.addCleanup(patch.stopall)

        # Create a mock socket instance and make sure it is returned by the
        # mock socket constructor.
        self._mock_socket_instance = MagicMock()
        self._mock_socket_constructor.return_value = \
            self._mock_socket_instance

        # Create a mock SSLContext instance and make sure it is returned by
        # the mock create_default_context method.
        self._mock_ssl_context = MagicMock()
        self._mock_create_default_context.return_value = \
            self._mock_ssl_context

        # Create a mock ssl socket and return it from SSLContext.wrap_socket().
        self._mock_ssl_socket = MagicMock()
        self._mock_ssl_context.wrap_socket.return_value = \
            self._mock_ssl_socket

    def test_network_link_create(self):
        """
        Test that network link is created correctly.
        """

        # Create the network link.
        NetworkLink(
            self._test_host_name,
            self._test_port_num,
            self._test_ca_cert,
            self._test_cert,
            self._test_key
        )

        # Make sure the ssl context was created with correct parameters.
        self._mock_create_default_context.assert_called_with(
            cafile=self._test_ca_cert
        )

        # Make sure the certificate and private key were loaded correctly.
        self._mock_ssl_context.load_cert_chain.assert_called_with(
            certfile=self._test_cert,
            keyfile=self._test_key
        )

        # Make sure the hostname checking is set correctly.
        self.assertFalse(self._mock_ssl_context.check_hostname)

        # Make sure the socket was created with correct parameters.
        self._mock_socket_constructor.assert_called_with(
            socket.AF_INET,
            socket.SOCK_STREAM
        )

        # Make sure the ssl context was used to wrap the socket.
        self._mock_ssl_context.wrap_socket.assert_called_with(
            self._mock_socket_instance
        )

    def test_network_link_connect(self):
        """
        Test that network link connects correctly.
        """

        # Create the network link and call connect().
        network_link = NetworkLink(
            self._test_host_name,
            self._test_port_num,
            self._test_ca_cert,
            self._test_cert,
            self._test_key
        )
        network_link.connect()

        # Make sure connect was called on the socket with correct
        # parameters.
        self._mock_ssl_socket.connect.assert_called_with(
            (self._test_host_name, self._test_port_num)
        )

    def test_network_link_disconnect(self):
        """
        Test that network link disconnects correctly.
        """

        # Create the network link and call connect() then disconnect.
        network_link = NetworkLink(
            self._test_host_name,
            self._test_port_num,
            self._test_ca_cert,
            self._test_cert,
            self._test_key
        )
        network_link.connect()
        network_link.disconnect()

        # Make sure disconnect was called on the socket.
        self._mock_ssl_socket.close.assert_called()

    def test_network_link_reconnect(self):
        """
        Test that network link reconnects correctly.
        """

        # Create a manager mock that we can use to verify the order of
        # calls is correct.
        mock_call_manager = MagicMock()
        mock_call_manager.attach_mock(self._mock_socket_constructor, 'create')
        mock_call_manager.attach_mock(self._mock_ssl_socket.connect, 'connect')
        mock_call_manager.attach_mock(self._mock_ssl_socket.close, 'close')

        # Create the network link and call connect() then reconnect.
        network_link = NetworkLink(
            self._test_host_name,
            self._test_port_num,
            self._test_ca_cert,
            self._test_cert,
            self._test_key
        )
        network_link._connected = True
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

    def test_network_link_send(self):
        """
        Test that network link sends data correctly.
        """

        # Create the network link.
        network_link = NetworkLink(
            self._test_host_name,
            self._test_port_num,
            self._test_ca_cert,
            self._test_cert,
            self._test_key
        )
        network_link._connected = True

        # Create a byte string and attempt to send it through the network link.
        network_link.send(self._test_data)

        # Make sure the correct data was passed to the underlying socket.
        self._mock_ssl_socket.sendall.assert_called_with(self._test_data)

    def test_network_link_receive(self):
        """
        Test that network link receives data correctly.
        """

        # Create some messages that we expect to receive.
        expected_data_values = [
            b'msg1',
            b'msg2',
            b'msg3',
            b'msg4',
            b'msg5',
            b'msg6',
        ]

        # Set the socket recv method to return the data in an unpredictable
        # manner such as may be seen when streaming bytes over the network.
        self._mock_ssl_socket.recv.side_effect = [
            b'msg1\nmsg2\nmsg3\n',
            b'msg',
            b'4',
            b'\n',
            b'msg5\nmsg',
            b'6\n'
        ]

        # Create the network link.
        network_link = NetworkLink(
            self._test_host_name,
            self._test_port_num,
            self._test_ca_cert,
            self._test_cert,
            self._test_key
        )
        network_link._connected = True

        # Repeatedly call the receive() method on the network link and verify
        # that the messages returned are correct.
        for expected_data in expected_data_values:
            actual_data = network_link.receive()
            # Make sure the recv() method of the underlying socket was called
            # and the data was returned as expected.
            self.assertEqual(expected_data, actual_data)


if __name__ == '__main__':
    unittest.main()
