#ifndef SOCKET_SERVER_H
#define SOCKET_SERVER_H

#include <netinet/in.h>
#include <sys/select.h>

#include <map>

#include <string>

using std::map;

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

        std::string get_address_str() const;

        int get_fd() const {
            return fd;
        }

        struct sockaddr_in get_address() const {
            return address;
        }
    };

    SocketServer() {}

    class ConnectionManager {
        public:
        ConnectionManager() : fdmax(0) {};
        ~ConnectionManager();
        void init();
        void add_connection(const Connection &connection);
        void remove_connection(const Connection &connection);
        void get_connections_fds(fd_set *dest) const;

        private:
        void add_fd(int fd);
        int fdmax;
        fd_set connection_fds;
        map<int, Connection*> active_connections; 
    };

};

}

#endif
