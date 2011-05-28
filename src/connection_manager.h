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
#ifndef CONNECTION_MANAGER_H
#define CONNECTION_MANAGEH_H

#include <sys/select.h>

#include <map>
#include <tr1/memory>

namespace sprawlnet {

class Connection;

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
    // Not copyable.
    ConnectionManager(const ConnectionManager &);
    ConnectionManager &operator=(const ConnectionManager &);

    void add_fd(int fd);
    void remove_fd(int fd);
    int fdmax;
    fd_set connection_fds;
    std::map<int, std::tr1::shared_ptr<Connection> > active_connections; 
};

} // namespace sprawlnet
#endif
