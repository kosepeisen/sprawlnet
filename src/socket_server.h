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
#ifndef SOCKET_SERVER_H
#define SOCKET_SERVER_H

#include <netinet/in.h>
#include <sys/select.h>

#include <tr1/memory>

namespace sprawlnet {

class Connection;
class ConnectionManager;
class MessageAssembler;

class SocketServer {
public:
    static SocketServer* create(MessageAssembler * const assembler);
    SocketServer(MessageAssembler * const assembler) 
            : assembler(assembler) {}
    virtual ~SocketServer();

    /** Call destroy() before deleting a SocketServer. */
    void destroy();
    
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

protected:
    void init();
    std::tr1::shared_ptr<ConnectionManager> all_connections;

    void close_all_connections();
    virtual void close_connection(const Connection &connection);

private:
    fd_set listener_sockets;
    MessageAssembler * const assembler;

    // Not copyable.
    SocketServer(const SocketServer &);
    SocketServer &operator=(const SocketServer &);

    bool try_bind_connection(const Connection &connection);

    /** 
     * Initialize a default hints argument.
     *
     * This struct will specify the default criteria for selecting the socket
     * address to listen to.
     */
    void init_hints(struct addrinfo *hints) const; 

    /** 
     * Enable reuseaddr on the socket.
     *
     * That way, we can bind to it several times, and we avoid the "address
     * already in use" issue.
     */
    void enable_reuseaddr(int fd) const;

    void handle_fd_activity(int fd);
    void accept_new_connection(const Connection &listener);
    void receive_from_connection(const Connection &connection);
};
}

#endif
