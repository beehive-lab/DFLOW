import socket

from client.interconnect.network import NetworkLink


# def receive(port):
#     with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as sock:
#         sock.bind(('', port))
#         sock.listen()
#         conn, addr = sock.accept()
#         with conn:
#             print('Connected by ', addr)
#             while True:
#                 data = conn.recv(1024)
#                 print('Received {}'.format(data))
#                 if not data:
#                     break
#                 conn.sendall(data)


# receive(8888)


with NetworkLink('localhost', 8888) as commlink:
    commlink.connect()
    commlink.send(b'data')
    commlink.disconnect()
