#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "string_util.h"
#include "socket_server.h"

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

namespace omninet {

std::string SocketServer::Connection::get_address() {
    std::string address_str = inet_ntoa(address.sin_addr);

    address_str += ":";
    int port = ntohs(address.sin_port);
    address_str += int_to_string(port);

    return address_str;
}

}
