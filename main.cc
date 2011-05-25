#include "socket_server.h"

using omninet::SocketServer;

int main() {
    SocketServer server;
    server.init();
    server.bind("3000");
    return 0;
}
