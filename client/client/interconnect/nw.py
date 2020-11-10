import socket


def send(host, port, data):
    print(
        'Sending {} to host {} on port {}'.format(
            data,
            host,
            port
        )
    )
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        s.connect((host, port))
        s.sendall(data)


def receive(host, port):
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as sock:
        sock.bind((host, port))
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


