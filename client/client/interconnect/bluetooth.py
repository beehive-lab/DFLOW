"""
NOTE: Currently bluetooth connectivity is only available on linux.
"""

import socket
import ssl

from .commlink import CommLink


class BluetoothLink(CommLink):
    """
    A class representing a communications link with a server over bluetooth.
    """

    def __init__(
        self,
        mac_addr: str,
        channel: int,
        ca_cert_file: str,
        cert_file: str,
        key_file: str,
        verify_host_name: bool = False,
        secure: bool = True,
        timeout: int = 20
    ):
        self._connected = False
        self._mac_addr: str = mac_addr
        self._channel: int = channel
        self._ca_cert_file: str = ca_cert_file
        self._cert_file: str = cert_file
        self._key_file: str = key_file
        self._verify_host_name = verify_host_name
        self._timeout = timeout
        self._secure = secure
        self._sock: socket.socket = self._create_bluetooth_socket()

    def _create_bluetooth_socket(self):
        return (
            self._create_secure_bluetooth_socket()
            if self._secure else
            self._create_plain_bluetooth_socket()
        )

    def _create_secure_bluetooth_socket(self) -> socket.socket:
        ssl_context: ssl.SSLContext = ssl.create_default_context(
            cafile=self._ca_cert_file
        )
        ssl_context.load_cert_chain(
            certfile=self._cert_file,
            keyfile=self._key_file
        )
        ssl_context.check_hostname = self._verify_host_name

        return ssl_context.wrap_socket(
            self._create_plain_bluetooth_socket()
        )

    def _create_plain_bluetooth_socket(self) -> socket.socket:
        sock: socket.socket = socket.socket(
            socket.AF_BLUETOOTH,
            socket.SOCK_STREAM,
            socket.BTPROTO_RFCOMM
        )
        sock.settimeout(self._timeout)
        return sock

    def __enter__(self):
        return self

    def __exit__(self, exc_type, exc_value, exc_traceback):
        self.disconnect()

    def connect(self) -> None:
        self._sock.connect((self._mac_addr, self._channel))
        self._connected = True

    def reconnect(self, secure=None) -> None:
        if secure is not None:
            self._secure = secure
        self.disconnect()
        self._sock = self._create_bluetooth_socket()
        self.connect()

    def disconnect(self) -> None:
        if self._connected:
            self._connected = False
            self._sock.settimeout(1)  # Interrupt any recv going on.
            self._sock.close()

    def send(self, data: bytes) -> None:
        self._sock.sendall(data)

    def receive(self, buffer_size: int = 1024) -> bytes:
        return self._sock.recv(buffer_size)

    def is_connected(self) -> bool:
        return self._connected
