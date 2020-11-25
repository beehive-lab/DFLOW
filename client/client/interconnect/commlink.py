from abc import ABC, abstractmethod


class CommLink(ABC):

    @abstractmethod
    def connect(self):
        pass

    @abstractmethod
    def disconnect(self):
        pass

    @abstractmethod
    def reconnect(self):
        pass

    @abstractmethod
    def send(self, data: bytes):
        pass

    @abstractmethod
    def receive(self) -> bytes:
        pass
