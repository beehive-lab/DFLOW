import socket

from .commlink import CommLink


class BluetoothLink(CommLink):
    """
    A class representing a communications link with a server over bluetooth.
    """

    def __init__(self, mac_addr: str, channel: int):
        self.mac_addr: str = mac_addr
        self.channel: int = channel
        self.sock: socket.socket = self._create_socket()

    def _create_socket(self) -> socket.socket:
        return socket.socket(
            socket.AF_BLUETOOTH,
            socket.SOCK_STREAM,
            socket.BTPROTO_RFCOMM
        )

    def __enter__(self):
        return self

    def __exit__(self, exc_type, exc_value, exc_traceback):
        self.sock.close()

    def connect(self) -> None:
        self.sock.connect((self.mac_addr, self.channel))

    def reconnect(self) -> None:
        self.sock.close()
        self.sock = self._create_socket()
        self.connect()

    def disconnect(self) -> None:
        self.sock.close()

    def send(self, data: bytes) -> None:
        self.sock.sendall(data)

    def receive(self) -> bytes:
        return self.sock.recv(1024)
