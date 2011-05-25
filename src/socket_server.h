#ifndef SOCKET_SERVER_H
#define SOCKET_SERVER_H

#include <netinet/in.h>
#include <sys/select.h>

#include <tr1/memory>

#include <map>

#include <string>

using std::map;
using std::tr1::shared_ptr;

namespace sprawlnet {

class SocketServer {
    public:
    class Connection {
        int fd;
        struct sockaddr_in address;

        public:
        Connection() : fd(-1) {}
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

        void copy_to(Connection *dest) const {
            dest->fd = fd;
            dest->address = address;
        }
    };

    class ConnectionManager {
        public:
        ConnectionManager() : fdmax(0) {};
        ~ConnectionManager();

        static ConnectionManager* create();

        /**
         * Get a connection.
         *
         * Creates a copy of the connection in `dest'.
         *
         * @returns true if the connection was copied, false otherwise.
         */
        bool get_connection(int fd, Connection *dest) const;
        void add_connection(Connection *connection);
        void remove_connection_by_fd(int fd);
        void remove_connection(const Connection *connection);
        void get_connections_fds(fd_set *dest) const;
        void init();

        private:
        void add_fd(int fd);
        void remove_fd(int fd);
        int fdmax;
        fd_set connection_fds;
        map<int, shared_ptr<Connection> > active_connections; 
    };

    SocketServer() {}
    void init();
    
    /**
     * Bind to a port.
     *
     * Tries to bind to `port' on both IPv4 and IPv6.
     *
     * @returns The number of addresses bound to.
     */
    int bind(const char *port);

    private:
    shared_ptr<ConnectionManager> connection_manager;
    fd_set listener_sockets;

    /** 
     * Initialize a default hints argument.
     *
     * This struct will specify the default criteria for selecting the socket
     * address to listen to.
     */
    void init_hints(struct addrinfo *hints); 
};

}

#endif
