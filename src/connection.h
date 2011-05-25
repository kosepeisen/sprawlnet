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
};

} // namespace sprawlnet
#endif
