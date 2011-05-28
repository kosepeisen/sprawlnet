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
#ifndef CONNECTION_H
#define CONNECTION_H

#include <netinet/in.h>
#include <sys/select.h>

#include <string>

namespace sprawlnet {

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

private:
    // Not copyable.
    Connection(const Connection &);
    Connection &operator=(const Connection &);
};

} // namespace sprawlnet
#endif
