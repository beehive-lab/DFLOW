import socket
import select

from .commlink import CommLink

# Commonly used flag sets
READ_ONLY = select.POLLIN | select.POLLPRI | select.POLLHUP | select.POLLERR
READ_WRITE = READ_ONLY | select.POLLOUT


class NetworkLink(CommLink):

    def __init__(self, host: str, port: int):
        self.host: str = host
        self.port: int = port
        self.sock: socket.socket = self._create_socket()

    def _create_socket(self):
        return socket.socket(
            socket.AF_INET,
            socket.SOCK_STREAM
        )

    def __enter__(self):
        return self

    def __exit__(self, exc_type, exc_value, exc_traceback):
        self.sock.close()

    def connect(self):
        self.sock.connect((self.host, self.port))

    def reconnect(self):
        self.sock.close()
        self.sock = self._create_socket()
        self.connect()

    def disconnect(self):
        self.sock.close()

    def send(self, data: bytes):
        self.sock.sendall(data)

    def receive(self) -> bytes:
        return self.sock.recv(1024)
