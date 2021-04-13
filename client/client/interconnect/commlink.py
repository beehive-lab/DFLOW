import socket
from abc import ABC, abstractmethod


class CommLink(ABC):
    """
    An abstract class representing a communications link with a server.
    """

    @abstractmethod
    def connect(self) -> None:
        pass

    @abstractmethod
    def disconnect(self) -> None:
        pass

    @abstractmethod
    def reconnect(self, secure=None) -> None:
        pass

    @abstractmethod
    def send(self, data: bytes) -> None:
        pass

    @abstractmethod
    def receive(self, buffer_size: int = 1024) -> bytes:
        pass

    @abstractmethod
    def is_connected(self) -> bool:
        pass

    @abstractmethod
    def ready_for_read(self) -> bool:
        pass

    @abstractmethod
    def get_raw_socket(self) -> socket.socket:
        pass
