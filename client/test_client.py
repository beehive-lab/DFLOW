import sys

from client.interconnect.bluetooth import BluetoothLink
from client.interconnect.commlink import CommLink
from client.interconnect.network import NetworkLink

if __name__ == "__main__":
    ca_cert = '../test_certs/rootCA/rootCA.crt'
    client_cert = '../test_certs/client/client.crt'
    client_key = '../test_certs/client/client.key'

    if len(sys.argv) == 2 and sys.argv[1] == 'b':
        print('starting bluetooth client')
        mac = input('mac: ').strip()
        channel = int(input('channel: ').strip())

        link: CommLink = BluetoothLink(
            mac,
            channel,
            ca_cert,
            client_cert,
            client_key
        )
    else:
        print('starting network client')
        host = input('host: ').strip()
        port = int(input('port: ').strip())

        link: CommLink = NetworkLink(
            host,
            port,
            ca_cert,
            client_cert,
            client_key
        )

    try:
        link.connect()
        while True:
            next_cmd = input('cmd:').strip().lower()
            if ':' in next_cmd:
                cmd, arg = next_cmd.split(':', maxsplit=1)
            else:
                cmd, arg = next_cmd, None
            if cmd == 'exit':
                break
            elif cmd == 'disconnect':
                link.disconnect()
            elif cmd == 'reconnect':
                link.reconnect()
            elif cmd == 'send':
                link.send(arg.encode())
            elif cmd == 'recv':
                print(link.receive())
    finally:
        link.disconnect()
