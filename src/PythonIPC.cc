#include "PythonIPC.h"

explicit PythonIPC::PythonIPC() : context(), socket(context, zmqpp::socket_type::req) {
    std::cout << "Connecting to hello world server at " << endpoint << "…" << '\n';
    socket.connect(endpoint);
}

PythonIPC::~PythonIPC() {
  socket.disconnect("tcp://localhost:5555");
}

PythonIPC::sendAndReceive(const string& message) {
  std::cout << "Sending: " << message << '\n';
  zmqpp::message request;
  request << message;

  // Send the message awaiting response
  socket.send(request);

  zmqpp::message response;
  socket.receive(response);
  std::string reply;
  response >> reply;

  std::cout << "Received: " << reply << '\n';
  return reply;
}
