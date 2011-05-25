#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/select.h>
#include <sys/types.h>
#include <netdb.h>
#include <string.h>

#include <tr1/memory>
#include <string>

#include "gtest/gtest.h"

#include "socket_server.h"

using std::string;
using std::tr1::shared_ptr;

namespace sprawlnet {

TEST(SocketServer_Connection, set_addr) {
    SocketServer::Connection c;
    const int expected = 1337;
    c.set_address((const struct sockaddr*)&expected, sizeof(expected));
    int result;
    c.get_address((struct sockaddr*)&result);
    EXPECT_EQ(expected, result);
}

TEST(SocketServer_Connection, get_addr_str) {
    SocketServer::Connection c;
    struct addrinfo *addr;
    int status = getaddrinfo("127.0.0.1", "1337", 0, &addr);
    ASSERT_EQ(0, status);

    c.set_address(addr->ai_addr, addr->ai_addrlen);

    EXPECT_EQ(string("127.0.0.1 port(1337)"), c.get_address_str());
}

TEST(SocketServer_ConnectionManager, add_connection) {
    shared_ptr<SocketServer::ConnectionManager>
            manager(SocketServer::ConnectionManager::create());

    SocketServer::Connection conn1(1);
    SocketServer::Connection conn2(2);

    manager->add_connection(conn1);
    manager->add_connection(conn2);

    fd_set fds;
    manager->get_connections_fds(&fds);

    EXPECT_TRUE(FD_ISSET(1, &fds));
    EXPECT_TRUE(FD_ISSET(2, &fds));
}

TEST(SocketServer_ConnectionManager, remove_connection) {
    shared_ptr<SocketServer::ConnectionManager>
            manager(SocketServer::ConnectionManager::create());

    SocketServer::Connection conn1(1);
    SocketServer::Connection conn2(2);

    manager->add_connection(conn1);
    manager->add_connection(conn2);

    fd_set fds;
    manager->get_connections_fds(&fds);

    EXPECT_TRUE(FD_ISSET(1, &fds));
    EXPECT_TRUE(FD_ISSET(2, &fds));

    manager->remove_connection(conn2);
    manager->get_connections_fds(&fds);

    EXPECT_TRUE(FD_ISSET(1, &fds));
    EXPECT_FALSE(FD_ISSET(2, &fds));

    manager->remove_connection(conn1);
    manager->get_connections_fds(&fds);

    EXPECT_FALSE(FD_ISSET(1, &fds));
    EXPECT_FALSE(FD_ISSET(2, &fds));
}

TEST(SocketServer_ConnectionManager, get_connection) {
    shared_ptr<SocketServer::ConnectionManager>
            manager(SocketServer::ConnectionManager::create());

    SocketServer::Connection conn(1);

    manager->add_connection(conn);

    SocketServer::Connection conn_;
    manager->get_connection(1, &conn_);
    EXPECT_EQ(1, conn_.get_fd());
}

}
