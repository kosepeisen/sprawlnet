#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/select.h>

#include <memory>
#include <string>

#include "gtest/gtest.h"

#include "socket_server.h"

using std::auto_ptr;

namespace omninet {

TEST(SocketServer_Connection, get_address) {
    std::string test_addr = "172.43.5.19";        
    uint16_t port = 1234;
    std::string port_str = "1234";

    struct sockaddr_in test_sockaddr;
    test_sockaddr.sin_family = AF_INET;
    test_sockaddr.sin_port = htons(port);
    ASSERT_NE(0, inet_pton(AF_INET, test_addr.c_str(),
            &test_sockaddr.sin_addr));

    SocketServer::Connection connection =
            SocketServer::Connection(0, test_sockaddr);

    std::string expected_test_addr_string = test_addr + ":" + port_str;
    EXPECT_EQ(expected_test_addr_string, connection.get_address_str());
}

TEST(SocketServer_ConnectionManager, add_connection) {
    auto_ptr<SocketServer::ConnectionManager>
            manager(SocketServer::ConnectionManager::create());

    struct sockaddr_in unused_address;
    SocketServer::Connection *conn1 =
            new SocketServer::Connection(1, unused_address);
    SocketServer::Connection *conn2 =
            new SocketServer::Connection(2, unused_address);

    manager->add_connection(conn1);
    manager->add_connection(conn2);

    fd_set fds;
    manager->get_connections_fds(&fds);

    EXPECT_TRUE(FD_ISSET(1, &fds));
    EXPECT_TRUE(FD_ISSET(2, &fds));
}

}
