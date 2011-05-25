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

class Connection;
class ConnectionManager;

class SocketServer {
    public:
    static SocketServer* create();

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
    void accept_new_connection(const Connection &listener);
    void receive_from_connection(const Connection &connection);
    void close_connection(const Connection &connection);
};
}

#endif
