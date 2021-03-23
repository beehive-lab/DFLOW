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
        verify_host_name: bool = False
    ):
        self._mac_addr: str = mac_addr
        self._channel: int = channel
        self._ca_cert_file: str = ca_cert_file
        self._cert_file: str = cert_file
        self._key_file: str = key_file
        self._verify_host_name = verify_host_name
        self._sock: socket.socket = self._create_secure_socket()

    def _create_secure_socket(self) -> socket.socket:
        ssl_context: ssl.SSLContext = ssl.create_default_context(
            cafile=self._ca_cert_file
        )
        ssl_context.load_cert_chain(
            certfile=self._cert_file,
            keyfile=self._key_file
        )
        ssl_context.check_hostname = self._verify_host_name

        return ssl_context.wrap_socket(
            socket.socket(
                socket.AF_BLUETOOTH,
                socket.SOCK_STREAM,
                socket.BTPROTO_RFCOMM
            )
        )

    def __enter__(self):
        return self

    def __exit__(self, exc_type, exc_value, exc_traceback):
        self._sock.close()

    def connect(self) -> None:
        self._sock.connect((self._mac_addr, self._channel))

    def reconnect(self) -> None:
        self._sock.close()
        self._sock = self._create_secure_socket()
        self.connect()

    def disconnect(self) -> None:
        self._sock.close()

    def send(self, data: bytes) -> None:
        self._sock.sendall(data)

    def receive(self, buffer_size: int = 1024) -> bytes:
        return self._sock.recv(buffer_size)
