import socket

from .commlink import CommLink


class BluetoothLink(CommLink):

    def __init__(self, mac_addr: str, port: int):
        self.host: str = mac_addr
        self.port: int = port
        self.sock: socket.socket = self._create_socket()

    def _create_socket(self):
        return socket.socket(
            socket.AF_BLUETOOTH,
            socket.SOCK_STREAM,
            socket.BTPROTO_RFCOMM
        )

    def __enter__(self):
        return self

    def __exit__(self, exc_type, exc_value, exc_traceback):
        self.sock.close()

    def connect(self):
        self.sock.connect((self.mac_addr, self.port))

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
