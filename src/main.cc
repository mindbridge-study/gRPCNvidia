#include "Service.h"

constexpr uint16_t port = 50051; // Default port

int main(int argc, char **argv) {
  ServerImpl server;
  server.Run(port);

  return 0;
}
