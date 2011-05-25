#include "socket_server.h"

using sprawlnet::SocketServer;

int main() {
    SocketServer server;
    server.init();
    server.bind("3000");
    server.listen();
    return 0;
}
