import bluetooth


def scan() -> []:
    return bluetooth.discover_devices(
        duration=8,
        lookup_names=True,
        flush_cache=True,
        lookup_class=False
    )


def send(adr: str, port: int, data: any):
    bd_addr = adr
    port = port
    sock = bluetooth.BluetoothSocket(bluetooth.RFCOMM)
    sock.connect((bd_addr, port))
    sock.send(data)
    sock.close()


def receive(port: int) -> any:
    server_sock = bluetooth.BluetoothSocket(bluetooth.RFCOMM)
    server_sock.bind(("", port))
    server_sock.listen(1)
    client_sock, address = server_sock.accept()
    data = client_sock.recv(1024)
    client_sock.close()
    server_sock.close()
    return data
