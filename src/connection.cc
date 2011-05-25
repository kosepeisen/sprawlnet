#include <netdb.h>
#include <stdlib.h>
#include <string.h>

#include "connection.h"

using std::string;

namespace sprawlnet {

Connection::~Connection() {
    if (address_len) {
        free(address);
    }
}

string Connection::get_address_str() const {
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


void Connection::set_address(const struct sockaddr *address,
        socklen_t address_len) {
    if (this->address_len) {
        free(this->address);
    }
    this->address = (struct sockaddr*)malloc(address_len);
    this->address_len = address_len;
    memcpy(this->address, address, address_len);
}

socklen_t Connection::get_address(struct sockaddr *dest) const {
    memcpy(dest, address, address_len);
    return address_len;
}

void Connection::copy_to(Connection *dest) const {
    dest->fd = fd;
    dest->set_address(address, address_len);
}

} // namespace sprawlnet
