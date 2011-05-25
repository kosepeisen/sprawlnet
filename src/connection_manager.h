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
