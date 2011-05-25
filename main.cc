#include "socket_server.h"
#include <memory>

using std::auto_ptr;
using sprawlnet::SocketServer;

int main() {
    auto_ptr<SocketServer> server(SocketServer::create());
    server->bind("3000");
    server->listen();
    return 0;
}
