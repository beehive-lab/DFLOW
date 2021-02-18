import socket
import ssl

if __name__ == "__main__":
    context = ssl.SSLContext(ssl.PROTOCOL_TLS_SERVER)
    context.load_cert_chain(
        '/home/osher/University/repos/DFLOW/test_certs/on-board/on-board.crt',
        '/home/osher/University/repos/DFLOW/test_certs/on-board/on-board.key'
    )
    context.load_verify_locations(
        cafile='/home/osher/University/repos/DFLOW/test_certs/rootCA/rootCA.crt'
    )
    context.verify_mode = ssl.CERT_REQUIRED

    with socket.socket(
        socket.AF_INET,
        socket.SOCK_STREAM,
    ) as sock:
        sock.bind(('127.0.0.1', 8443))
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
