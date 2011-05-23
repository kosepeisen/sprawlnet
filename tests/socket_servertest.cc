#include <arpa/inet.h>

#include <string>

#include "gtest/gtest.h"

#include "socket_server.h"

namespace omninet {

TEST(SocketServer, get_address) {
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
    EXPECT_EQ(expected_test_addr_string, connection.get_address());
}

}
