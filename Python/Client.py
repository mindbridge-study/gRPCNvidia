#
#   Hello World server in Python
#   Binds REP socket to tcp://*:5555
#   Expects b"Hello" from client, replies with b"World"
#

import zmq

context = zmq.Context()
socket = context.socket(zmq.REP)
socket.bind("tcp://*:5555")

class Client:
    def __init__(self, TcpAddress: str):
        self.context = zmq.Context()
        self.socket = context.socket(zmq.REP)
        self.socket.bind(TcpAddress)
        self.poller = zmq.Poller()
        self.poller.register(self.socket, zmq.POLLIN)

    def BlockingRecv(self):
        while True:
            events = dict(self.poller.poll())  # This will block until a message is available
            if self.socket in events:
                return self.socket.recv()

    def AsyncSend(self, Data):
        self.socket.send(Data)

if __name__ == "__main__":
    server = Client("tcp://*:5555")
    while True:
        message = server.BlockingRecv()
        print(f"Received: {message}")
        server.AsyncSend(b"World")

