import socket

from .commlink import CommLink


class NetworkLink(CommLink):
    """
    A class representing a communications link with a server over a network.
    """

    def __init__(self, host: str, port: int):
        self._host: str = host
        self._port: int = port
        self._sock: socket.socket = self._create_socket()

    def _create_socket(self) -> socket.socket:
        return socket.socket(
            socket.AF_INET,
            socket.SOCK_STREAM
        )

    def __enter__(self):
        return self

    def __exit__(self, exc_type, exc_value, exc_traceback):
        self._sock.close()

    def connect(self) -> None:
        self._sock.connect((self._host, self._port))

    def reconnect(self) -> None:
        self._sock.close()
        self._sock = self._create_socket()
        self.connect()

    def disconnect(self) -> None:
        self._sock.close()

    def send(self, data: bytes) -> None:
        self._sock.sendall(data)

    def receive(self) -> bytes:
        return self._sock.recv(1024)
