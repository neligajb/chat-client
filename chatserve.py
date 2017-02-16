import socket
import struct
import sys

def get_initial_cxn(listen_port):
    serversocket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    host = socket.gethostname()
    serversocket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)  # allow socket reuse
    serversocket.bind(('', int(listen_port)))
    serversocket.listen(5)                                              # queue up to 5 requests
    clientsocket, addr = serversocket.accept()

    data = clientsocket.recv(1024)
    print("Incoming connection from %s" % str(addr))
    status = struct.unpack("!i", data)[0]
    print "Status:", status
    return(clientsocket, host)


def send_msg(clientsocket, host):
    get_msg = True
    while get_msg:
        new_msg = raw_input(">> ")
        if len(new_msg) <= 500:
            get_msg = False
        else:
            print "Error: 500 character limit\n"

    new_msg = host + "> " + new_msg
    clientsocket.send(new_msg)
    if '\quit' in new_msg:
        return False
    return True


def receive_msg(clientsocket):
    data = clientsocket.recv(1024)
    if '\quit' in data:
        print "Client closed connection"
        return False
    print data
    return True


if __name__ == '__main__':
    if len(sys.argv) != 2:
        sys.stderr.write("usage: chatserve.py [portnum]\n")
        exit(1)

    listen_port = sys.argv[1]
    clientsocket, host = get_initial_cxn(listen_port)

    while 1:
        stay_alive = True
        stay_alive = send_msg(clientsocket, host)
        if not stay_alive:
            clientsocket.close()
            clientsocket, host = get_initial_cxn(listen_port)
            continue
        stay_alive = receive_msg(clientsocket)
        if not stay_alive:
            clientsocket.close()
            clientsocket, host = get_initial_cxn(listen_port)
