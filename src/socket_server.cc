/**
 * Copyright 2011 Kosepeisen.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License. See accompanying LICENSE file.
 */
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#include "connection.h"
#include "connection_manager.h"
#include "message_assembler.h"
#include "socket_server.h"

using std::string;

namespace sprawlnet {

void error(const char *msg) {
    perror(msg);
    exit(1);
}

SocketServer *SocketServer::create(MessageAssembler * const assembler) {
    SocketServer* socketServer = new SocketServer(assembler);
    socketServer->init();
    return socketServer;
}

SocketServer::~SocketServer() {
    close_all_connections();
}

void SocketServer::init() {
    all_connections.reset(ConnectionManager::create());
    FD_ZERO(&listener_sockets);
}

int SocketServer::bind(const char *port) {
    int status;
    int sockets_bound = 0;
    struct addrinfo hints;
    init_hints(&hints);

    struct addrinfo *result, *rp;
    status = getaddrinfo(NULL, port, &hints, &result);
    if (status != 0) {
        fprintf(stderr, "selectserver: %s\n", gai_strerror(status));
    }

    for (rp = result; rp != NULL; rp = rp->ai_next) {
        int fd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        enable_reuseaddr(fd);
        if (fd == -1) {
            perror("Could not create socket.");
        } else {
            Connection connection(fd);
            connection.set_address(rp->ai_addr, rp->ai_addrlen);
            status = try_bind_connection(connection);
            if (status) {
                sockets_bound++;
            }
        }
    }

    freeaddrinfo(result);
    return sockets_bound;
}

bool SocketServer::try_bind_connection(const Connection &connection) {
    int address_length = connection.get_address_length();
    struct sockaddr *address = (struct sockaddr*)malloc(address_length);
    connection.get_address(address);

    int status = ::bind(connection.get_fd(), address, address_length);
    bool result = false;
    if (status == -1) {
        close(connection.get_fd());
        printf("Could not bind to socket with address %s\n",
                connection.get_address_str().c_str());
        perror("bind()");
        result = false;
    } else {
        status = ::listen(connection.get_fd(), 10);
        if (status == -1) {
            printf("Could not listen with address %s\n",
                    connection.get_address_str().c_str());
            perror("listen() failed");
            result = false;
        } else {
            printf("Bound to %s.\n", connection.get_address_str().c_str());

            all_connections->add_connection(connection);
            FD_SET(connection.get_fd(), &listener_sockets);
            result = true;
        }
    }

    free(address);
    return result;
}

void SocketServer::init_hints(struct addrinfo *hints) const {
    memset(hints, 0, sizeof(struct addrinfo));
    hints->ai_family = AF_UNSPEC;
    hints->ai_socktype = SOCK_STREAM;
    hints->ai_flags = AI_PASSIVE;
    hints->ai_protocol = 0;
    hints->ai_canonname = NULL;
    hints->ai_addr = NULL;
    hints->ai_next = NULL;
}

void SocketServer::listen() {
    int status;
    fd_set sockets_copy;
    struct timeval timeout = { 30, 0 };

    while (all_connections->has_connections()) {
        all_connections->get_connections_fds(&sockets_copy);
        status = select(all_connections->get_fdmax() + 1,
                &sockets_copy, NULL, NULL, &timeout);
        if (status == -1) {
            perror("Select() failed.");
            return;
        }

        for (int fd = 0; fd <= all_connections->get_fdmax(); fd++) {
            if (FD_ISSET(fd, &sockets_copy)) {
                handle_fd_activity(fd);
            }
        }
    }
}

void SocketServer::handle_fd_activity(int fd) {
    int status;
    Connection connection;

    status = all_connections->get_connection(fd, &connection);
    if (!status) {
        printf("No connection found for file descriptor %d", fd);
        return;
    }
    
    if (FD_ISSET(fd, &listener_sockets)) {
        printf("Activity on %s. Open new connection.\n", connection.get_address_str().c_str());
        accept_new_connection(connection);
    } else {
        printf("Activity on connection %s.\n", connection.get_address_str().c_str());
        receive_from_connection(connection);
    }
}

void SocketServer::accept_new_connection(const Connection &listener) {
    struct sockaddr_storage remote_addr;
    socklen_t addrlen = sizeof(remote_addr);
    
    int new_fd = accept(listener.get_fd(), (struct sockaddr*)&remote_addr, 
            &addrlen);
    if (new_fd == -1) {
        perror("Could not accept connection. accept()");
    } else {
        Connection new_connection(new_fd);
        new_connection.set_address((const struct sockaddr*)&remote_addr,
                addrlen);
        all_connections->add_connection(new_connection);
        printf("Connection accepted from %s\n", 
                new_connection.get_address_str().c_str());
    }
}

void SocketServer::receive_from_connection(const Connection &connection) {
    const size_t BUFSIZE = 1024;
    char buffer[BUFSIZE+1];
    int num_bytes_read = recv(connection.get_fd(), buffer, BUFSIZE, 0);
    if (num_bytes_read < 0) {
        perror("Could not receive from socket: recv()");
        close_connection(connection);
    } else if (num_bytes_read == 0) {
        printf("Connection closed.\n");
        close_connection(connection);
    } else {
        buffer[num_bytes_read] = '\0';
        assembler->assemble(connection, buffer, num_bytes_read);
    }
}

void SocketServer::close_all_connections() {
    fd_set fds;
    Connection connection;
    all_connections->get_connections_fds(&fds);

    for (int fd = 0; fd <= all_connections->get_fdmax(); fd++) {
        if (FD_ISSET(fd, &fds)) {
            all_connections->get_connection(fd, &connection);
            close_connection(connection);
        }
    }
}

void SocketServer::close_connection(const Connection &connection) {
    printf("Closing connection with address %s\n",
            connection.get_address_str().c_str());
    close(connection.get_fd());

    all_connections->remove_connection(connection);
    if (FD_ISSET(connection.get_fd(), &listener_sockets)) {
        FD_CLR(connection.get_fd(), &listener_sockets);
    }
}

void SocketServer::enable_reuseaddr(int fd) const {
    int value = 1;
    int status = setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &value, sizeof(int));
    if (status == -1) {
        error("Failed to set SO_REUSEADDR on socket.");
    }
}

}
