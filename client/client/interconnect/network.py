import errno
import select
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
        verify_host_name: bool = False,
        secure: bool = True
    ):
        self._connected = False
        self._host: str = host
        self._port: int = port
        self._ca_cert_file: str = ca_cert_file
        self._cert_file: str = cert_file
        self._key_file: str = key_file
        self._verify_host_name: bool = verify_host_name
        self._secure: bool = secure
        self._sock: socket.socket = self._create_network_socket()
        self._leftover = b''

    def _create_network_socket(self):
        return (
            self._create_secure_network_socket()
            if self._secure else
            self._create_plain_network_socket()
        )

    def _create_secure_network_socket(self) -> socket.socket:
        ssl_context: ssl.SSLContext = ssl.create_default_context(
            cafile=self._ca_cert_file
        )
        ssl_context.load_cert_chain(
            certfile=self._cert_file,
            keyfile=self._key_file
        )
        ssl_context.check_hostname = self._verify_host_name

        return ssl_context.wrap_socket(
            self._create_plain_network_socket()
        )

    def _create_plain_network_socket(self) -> socket.socket:
        sock: socket.socket = socket.socket(
            socket.AF_INET,
            socket.SOCK_STREAM
        )
        return sock

    def __enter__(self):
        return self

    def __exit__(self, exc_type, exc_value, exc_traceback):
        self.disconnect()

    def connect(self) -> None:
        self._sock.connect((self._host, self._port))
        self._connected = True

    def reconnect(self, secure=None) -> None:
        if secure is not None:
            self._secure = secure
        self.disconnect()
        self._sock = self._create_network_socket()
        self.connect()

    def disconnect(self) -> None:
        if self.is_connected():
            self._connected = False
            self._sock.shutdown(socket.SHUT_RDWR)
            self._sock.close()

    def send(self, data: bytes) -> None:
        if not self.is_connected():
            return
        self._sock.sendall(data)

    def receive(self, buffer_size: int = 1024) -> bytes:
        # Check that the underlying socket is actually connected
        # before attempting a read.
        if not self.is_connected():
            return b''

        # If there is some data left over from a previous recv then
        # start with that.
        data_read = self._leftover

        try:
            # Loop until a we see a message separator indicating that we have
            # a full message in data_read.
            msg_end = data_read.find(b'\n')
            while msg_end == -1:
                data_read += self._sock.recv(buffer_size)
                msg_end = data_read.find(b'\n')

            # Any data following the message separator is saved for the
            # next receive().
            self._leftover = data_read[msg_end + 1:]

        except OSError as err:

            # Certain errors often occur when the socket is shutdown in the
            # middle of a recv so we want to handle this gracefully.
            # Otherwise we rethrow the error.
            if err.errno != errno.EBADF and err.errno != errno.ENOTCONN:
                raise

            # Mark the connection as no longer connected and return an
            # empty byte string.
            self._connected = False
            self._leftover = b''
            return b''

        # Return all the data up to the first message separator.
        return data_read[:msg_end]

    def is_connected(self) -> bool:
        return self._connected

    def ready_for_read(self) -> bool:
        if not self.is_connected():
            return False

        read, *_ = select.select([self._sock], [], [], 1)
        if read:
            return True
        else:
            return False

    def get_raw_socket(self):
        return self._sock
