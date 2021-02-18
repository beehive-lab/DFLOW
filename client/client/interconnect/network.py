import socket
import ssl

from .commlink import CommLink


class NetworkLink(CommLink):
    """
    A class representing a communications link with a server over a network.
    """

    def __init__(
        self,
        host: str,
        port: int,
        ca_cert_file: str,
        cert_file: str,
        key_file: str,
        verify_host_name: bool = False
    ):
        self._host: str = host
        self._port: int = port
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
                socket.AF_INET,
                socket.SOCK_STREAM
            )
        )

    def __enter__(self):
        return self

    def __exit__(self, exc_type, exc_value, exc_traceback):
        self._sock.close()

    def connect(self) -> None:
        self._sock.connect((self._host, self._port))

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
