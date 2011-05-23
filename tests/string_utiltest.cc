#include <arpa/inet.h>

#include <string>

#include "gtest/gtest.h"

#include "string_util.h"

namespace omninet {

TEST(Strings, int_to_string) {
    EXPECT_EQ("1234", int_to_string(1234));
    EXPECT_EQ("-12345678", int_to_string(-12345678));
    EXPECT_EQ("0", int_to_string(0));
}

}
