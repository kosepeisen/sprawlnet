#include "connection.h"
#include "connection_manager.h"

using std::map;
using std::tr1::shared_ptr;

namespace sprawlnet {

ConnectionManager *ConnectionManager::create() {
    ConnectionManager* manager = new ConnectionManager();
    manager->init();
    return manager;
}

ConnectionManager::~ConnectionManager() {
    // TODO: Implement this.
}

void ConnectionManager::init() {
    FD_ZERO(&connection_fds);
}

bool ConnectionManager::has_connections() const {
    return !active_connections.empty();
}

void ConnectionManager::get_connections_fds(fd_set *dest) const {
    *dest = connection_fds;
}

void ConnectionManager::add_connection(
        const Connection &connection) {
    shared_ptr<Connection> connection_(new Connection());
    connection.copy_to(connection_.get());

    add_fd(connection_->get_fd());
    active_connections[connection_->get_fd()] = connection_;
}

void ConnectionManager::remove_connection(
        const Connection &connection) {
    remove_connection_by_fd(connection.get_fd());
}

void ConnectionManager::remove_connection_by_fd(int fd) {
    map<int, shared_ptr<Connection> >::iterator it;
    it = active_connections.find(fd);
    if (it != active_connections.end()) {
        remove_fd(fd);
        active_connections.erase(it);
    }
}

void ConnectionManager::add_fd(int fd) {
    if (fd > fdmax) {
        fdmax = fd;
    }
    FD_SET(fd, &connection_fds);
}

void ConnectionManager::remove_fd(int fd) {
    // TODO: Do we have to update fdmax? I'm guessing no.
    FD_CLR(fd, &connection_fds);
}

bool ConnectionManager::get_connection(int fd,
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

} // namespace sprawlnet
