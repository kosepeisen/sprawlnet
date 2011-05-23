#ifndef SOCKET_SERVER_H
#define SOCKET_SERVER_H

#include <netinet/in.h>

#include <string>

namespace omninet {

class SocketServer {
    public:

    class Connection {
        int fd;
        struct sockaddr_in address;

        public:
        Connection(int fd, struct sockaddr_in address)
            : fd(fd),
              address(address) {}

        std::string get_address();
    };

    SocketServer() {}
};

}

#endif
