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
#include "message_parser.h"

#include "gtest/gtest.h"
#include <cstring>
#include <string>

#include "message/message_header.pb.h"

using std::string;

namespace sprawlnet {

class MessageParserTest : public ::testing::Test {
public:
    MessageParser message_parser;

    void test_get_header_length() {
        size_t message_length = 4;
        const char *message = reinterpret_cast<char*>(&message_length);
        EXPECT_EQ(message_length, message_parser.get_header_length(message));
    }

    void test_parse_header() {
        message::MessageHeader src;
        src.set_message_type("my message type");
        string message;
        ASSERT_TRUE(src.SerializeToString(&message));

        message::MessageHeader target;
        message_parser.parse_header(message.c_str(), message.size(), &target);
        EXPECT_EQ(src.message_type(), target.message_type());
    }
};

TEST_F(MessageParserTest, get_header_length) {
    test_get_header_length();
}

TEST_F(MessageParserTest, parse_header) {
    test_parse_header();
}

} // namespace sprawlnet
