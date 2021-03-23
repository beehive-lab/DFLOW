import socket
import ssl
import sys
import os

if __name__ == "__main__":
    context = ssl.SSLContext(ssl.PROTOCOL_TLS_SERVER)
    context.load_cert_chain(
        '../test_certs/on-board/on-board.crt',
        '../test_certs/on-board/on-board.key'
    )
    context.load_verify_locations(
        cafile='../test_certs/rootCA/rootCA.crt'
    )
    context.verify_mode = ssl.CERT_REQUIRED

    if len(sys.argv) == 2 and sys.argv[1] == 'b':
        bt_mac = os.popen('hcitool dev | cut -sf3').read().strip()
        bt_channel = 4
        print('starting bluetooth server on ', bt_mac, ':', bt_channel)
        with socket.socket(
            socket.AF_BLUETOOTH,
            socket.SOCK_STREAM,
            socket.BTPROTO_RFCOMM
        ) as sock:
            sock.bind((bt_mac, 4))
            sock.listen(5)
            with context.wrap_socket(sock, server_side=True) as ssock:
                conn, addr = ssock.accept()
                with conn:
                    while True:
                        data = conn.recv(1024)
                        if not data:
                            break
                        print('received: ', data)
                        conn.sendall(data)
    else:
        with socket.socket(
            socket.AF_INET,
            socket.SOCK_STREAM,
        ) as sock:
            host = '127.0.0.1'
            port = 8443
            print('starting network server on ', host, ':', port)
            sock.bind((host, port))
            sock.listen(5)
            with context.wrap_socket(sock, server_side=True) as ssock:
                conn, addr = ssock.accept()
                with conn:
                    while True:
                        data = conn.recv(1024)
                        if not data:
                            break
                        print('received: ', data)
                        conn.sendall(data)
