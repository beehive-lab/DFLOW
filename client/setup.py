import socket


def receive(port):
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as sock:
        sock.bind(('', port))
        sock.listen()
        conn, addr = sock.accept()
        with conn:
            print('Connected by ', addr)
            while True:
                data = conn.recv(1024)
                print('Received {}'.format(data))
                if not data:
                    break
                conn.sendall(data)


def receive_bluetooth(port):
    # The MAC address of a Bluetooth adapter on the server.
    # The server might have multiple Bluetooth adapters.
    hostMACAddress = '01:19:86:00:E0:C2'

    # 3 is an arbitrary choice.
    # However, it must match the port used by the client.
    backlog = 1
    size = 1024
    s = socket.socket(
        socket.AF_BLUETOOTH,
        socket.SOCK_STREAM,
        socket.BTPROTO_RFCOMM
    )
    s.bind((hostMACAddress, port))
    s.listen(backlog)
    try:
        client, address = s.accept()
        while 1:
            data = client.recv(size)
            if data:
                print(data)
                client.send(data)
    except:
        print("Closing socket")
        client.close()
        s.close()


# receive_bluetooth(3)
# receive(8888)


# with NetworkLink('localhost', 8888) as commlink:
#     commlink.connect()
#     commlink.send(b'data')
#     commlink.disconnect()

# with BluetoothLink('1C:4D:70:7D:41:45', 4) as commlink:
#     commlink.connect()
#     commlink.send(b'test')
#     commlink.disconnect()
