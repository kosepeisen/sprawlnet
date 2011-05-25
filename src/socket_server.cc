#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#include "string_util.h"
#include "socket_server.h"

using std::string;

const uint16_t PORT = 3000;
const int TIMEOUT = 30;

void error(const char *msg) {
    perror(msg);
    exit(1);
}

void enable_reuseaddr(int socket) {
    int value = 1;
    int status = setsockopt(socket, SOL_SOCKET, SO_REUSEADDR, &value, sizeof(int));
    if (status == -1) {
        error("Failed to set SO_REUSEADDR on socket.");
    }
}

#if 0
int main() {
    int status;

    int listener_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (listener_socket == -1) {
        error("Could not create listening socket.");
    }
    
    enable_reuseaddr(listener_socket);

    struct sockaddr_in bind_address;
    bind_address.sin_family = AF_INET;
    bind_address.sin_port = htons(PORT);
    status = inet_pton(AF_INET, "0.0.0.0", &bind_address.sin_addr);
    if (status == 0) {
        error("Invalid address.");
    } else if (status == -1) {
        error("Unknown adress family");
    }

    status = bind(listener_socket, (struct sockaddr*)&bind_address, sizeof(bind_address));
    if (status == -1) {
        error("Could not bind to socket");
    }

    const int connection_queue = 10;
    status = listen(listener_socket, connection_queue);
    if (status == -1) {
        error("Could not set socket as a passive socket.");
    }
    
    int fdmax = listener_socket;
    fd_set sockets;
    FD_ZERO(&sockets);
    FD_SET(listener_socket, &sockets);

    while (1) {
        fd_set sockets_copy = sockets;

        struct timeval timeout = { 1, 0 };
        status = select(fdmax+1, &sockets_copy, NULL, NULL, &timeout);
        if (status == -1) {
            error("Select() failed.");
        }

        for (int i = 0; i <= fdmax; i++) {
            if (FD_ISSET(i, &sockets_copy)) {
                if (listener_socket == i) {
                    struct sockaddr_in client_address;
                    socklen_t client_address_size = sizeof(client_address);
                    int client_socket = accept(listener_socket,
                            (struct sockaddr*)&client_address,
                            &client_address_size);
                    if (client_socket >= 0) {
                        char client_address_str[INET_ADDRSTRLEN];                        
                        char *client_address_str_tmp = inet_ntoa(client_address.sin_addr);
                        strcpy(client_address_str, client_address_str_tmp);

                        printf("Received connection from %s\n",
                                client_address_str);
                        FD_SET(client_socket, &sockets);
                        if (client_socket > fdmax) {
                            fdmax = client_socket;
                        }
                    } else {
                        perror("Could not accept connection.");
                    }
                } else {
                    const size_t BUFSIZE = 1024;
                    char buffer[BUFSIZE+1];
                    int num_bytes_read = recv(i, buffer, BUFSIZE, 0);
                    if (num_bytes_read < 0) {
                        perror("Could not receive from socket: recv()");
                        close(i);
                        FD_CLR(i, &sockets);
                    } else if (num_bytes_read == 0) {
                        printf("Connection closed.\n");
                        close(i);
                        FD_CLR(i, &sockets);
                    } else {
                        buffer[num_bytes_read] = '\0';
                        printf("Received data from client: %s\n", buffer); 
                    }
                }
            }
        }
    }
}
#endif

namespace sprawlnet {

SocketServer::Connection::~Connection() {
    if (address_len) {
        free(address);
    }
}

void SocketServer::Connection::set_address(const struct sockaddr *address,
        socklen_t address_len) {
    if (this->address_len) {
        free(this->address);
    }
    this->address = (struct sockaddr*)malloc(address_len);
    this->address_len = address_len;
    memcpy(this->address, address, address_len);
}

socklen_t SocketServer::Connection::get_address(struct sockaddr *dest) const {
    memcpy(dest, address, address_len);
    return address_len;
}

void SocketServer::Connection::copy_to(Connection *dest) const {
    dest->fd = fd;
    dest->set_address(address, address_len);
}

SocketServer::ConnectionManager* SocketServer::ConnectionManager::create() {
    ConnectionManager* manager = new ConnectionManager();
    manager->init();
    return manager;
}

string SocketServer::Connection::get_address_str() const {
    char host_buffer[NI_MAXHOST];
    char serv_buffer[NI_MAXSERV];

    int status = getnameinfo(address, address_len, host_buffer,
            sizeof(host_buffer), serv_buffer, sizeof(serv_buffer),
            NI_NUMERICHOST | NI_NUMERICSERV);
    
    if (status != 0) {
        return NULL;
    } else {
        return string() + host_buffer + " port(" + serv_buffer + ")";
    }
}

SocketServer::ConnectionManager::~ConnectionManager() {
    // TODO: Implement this.
}

void SocketServer::ConnectionManager::init() {
    FD_ZERO(&connection_fds);
}

void SocketServer::ConnectionManager::get_connections_fds(fd_set *dest) const {
    *dest = connection_fds;
}

void SocketServer::ConnectionManager::add_connection(Connection *connection) {
    add_fd(connection->get_fd());
    active_connections[connection->get_fd()] = shared_ptr<Connection>(connection);
}

void SocketServer::ConnectionManager::remove_connection(
        const Connection *connection) {
    remove_connection_by_fd(connection->get_fd());
}

void SocketServer::ConnectionManager::remove_connection_by_fd(int fd) {
    map<int, shared_ptr<Connection> >::iterator it;
    it = active_connections.find(fd);
    if (it != active_connections.end()) {
        remove_fd(fd);
        active_connections.erase(it);
    }
}

void SocketServer::ConnectionManager::add_fd(int fd) {
    if (fd > fdmax) {
        fdmax = fd;
    }
    FD_SET(fd, &connection_fds);
}

void SocketServer::ConnectionManager::remove_fd(int fd) {
    // TODO: Do we have to update fdmax? I'm guessing no.
    FD_CLR(fd, &connection_fds);
}

bool SocketServer::ConnectionManager::get_connection(int fd,
        Connection *dest) const {
    map<int, shared_ptr<Connection> >::const_iterator it;
    it = active_connections.find(fd);
    if (it != active_connections.end()) {
        it->second->copy_to(dest);
        return true;
    } else {
        return false;
    }
}

void SocketServer::init() {
    connection_manager.reset(new ConnectionManager());
}

int SocketServer::bind(const char *port) {
    int status;
    int sockets_bound = 0;
    struct addrinfo hints;
    init_hints(&hints);

    struct addrinfo *result, *rp;
    status = getaddrinfo(NULL, port, &hints, &result);

    for (rp = result; rp != NULL; rp = rp->ai_next) {
        int fd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if (fd == -1) {
            perror("Could not create socket.");
        } else {
            Connection connection(fd);
            connection.set_address(rp->ai_addr, rp->ai_addrlen);

            status = ::bind(fd, rp->ai_addr, rp->ai_addrlen);
            if (status == -1) {
                printf("Could not bind to socket with address %s\n",
                        connection.get_address_str().c_str());
                perror("bind()");
            } else {
                printf("Bound to %s.\n", connection.get_address_str().c_str());
                sockets_bound++;
                // TODO: Register fds.
            }
        }
    }

    freeaddrinfo(result);
    return sockets_bound;
}

void SocketServer::init_hints(struct addrinfo *hints) {
    memset(hints, 0, sizeof(struct addrinfo));
    hints->ai_family = AF_UNSPEC;
    hints->ai_socktype = SOCK_DGRAM;
    hints->ai_flags = AI_PASSIVE;
    hints->ai_protocol = 0;
    hints->ai_canonname = NULL;
    hints->ai_addr = NULL;
    hints->ai_next = NULL;
}

}
