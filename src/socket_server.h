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
        struct sockaddr *address;
        socklen_t address_len;

        public:
        Connection() : fd(-1), address_len(0) {}
        Connection(int fd) : fd(fd), address(NULL), address_len(0) {}
        ~Connection();

        std::string get_address_str() const;
        int get_fd() const { return fd; }
        void set_address(const struct sockaddr *address, socklen_t address_len);
        socklen_t get_address(struct sockaddr *dest) const;
        socklen_t get_address_length() const { return address_len; }
        void copy_to(Connection *dest) const;
    };

    class ConnectionManager {
        public:
        ConnectionManager() : fdmax(0) {};
        ~ConnectionManager();

        static ConnectionManager* create();

        bool has_connections() const;
        /**
         * Get a connection.
         *
         * Creates a copy of the connection in `dest'.
         *
         * @returns true if the connection was copied, false otherwise.
         */
        bool get_connection(int fd, Connection *dest) const;
        int get_fdmax() const { return fdmax; }
        void add_connection(const Connection &connection);
        void remove_connection_by_fd(int fd);
        void remove_connection(const Connection &connection);
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

    /**
     * Listen to the bound sockets.
     *
     * Returns when all sockets are closed.
     */
    void listen();

    private:
    shared_ptr<ConnectionManager> all_connections;
    fd_set listener_sockets;

    bool try_bind_connection(const Connection &connection);

    /** 
     * Initialize a default hints argument.
     *
     * This struct will specify the default criteria for selecting the socket
     * address to listen to.
     */
    void init_hints(struct addrinfo *hints); 

    /** 
     * Enable reuseaddr on the socket.
     *
     * That way, we can bind to it several times, and we avoid the "address
     * already in use" issue.
     */
    void enable_reuseaddr(int fd);

    void handle_fd_activity(int fd);
};
}

#endif
