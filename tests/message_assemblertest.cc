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
#include <string.h>

#include <iostream>
#include <map>
#include <tr1/memory>

#include "gtest/gtest.h"

#include "connection.h"
#include "message_assembler.h"
#include "message_parser_interface.h"

using std::cout;
using std::endl;
using std::map;
using std::tr1::shared_ptr;

namespace sprawlnet {

void generate_message(const char *message_str,
        int *message_length_dest, char **message_dest) {
    int length = strlen(message_str) + 1;

    *message_dest = new char[length + sizeof(int)]; 
    *message_length_dest = length + sizeof(int);

    memcpy(*message_dest, &length, sizeof(int));
    memcpy(*message_dest + sizeof(int), message_str, length);
}

class TestMessageParser : public MessageParserInterface {
public:
    char *message;
    int message_length;

    TestMessageParser() : message(NULL), message_length(0) {}

    void parse(const char *buffer, size_t buffer_size) {
        message = new char[buffer_size];
        memcpy(message, buffer, buffer_size);
        message_length = buffer_size;
    }
};

class TestMessageAssembler : public MessageAssembler {
public:
    TestMessageAssembler(MessageParserInterface *const parser)
            : MessageAssembler(parser) {
    }

    PartialMessage *test_get_partial_message(const Connection &connection) {
        return get_partial_message(connection);
    }

    map<int, shared_ptr<PartialMessage> > &test_get_partial_messages() {
        return partial_messages;
    }
};

class MessageAssemblerTest : public ::testing::Test {
public:
        Connection connection;
        TestMessageParser parser;
        TestMessageAssembler assembler;

        MessageAssemblerTest() : connection(1), assembler(&parser) {}
};

TEST_F(MessageAssemblerTest, close_connection) {
    PartialMessage *partial = new PartialMessage();
    assembler.test_get_partial_messages()[1] =
            shared_ptr<PartialMessage>(partial);

    EXPECT_EQ(partial, assembler.test_get_partial_message(connection));
    assembler.close_connection(connection);
    EXPECT_TRUE(assembler.test_get_partial_message(connection) == NULL);
}

TEST_F(MessageAssemblerTest, assemble) {
    char *message;
    int message_length;
    generate_message("This is a test message.", &message_length, &message);

    assembler.assemble(connection, message, message_length);

    EXPECT_STREQ("This is a test message.", parser.message);

    delete message;
}

TEST_F(MessageAssemblerTest, assemble_fragments) {
    char *message;
    int message_length;
    generate_message("This is a test message.", &message_length, &message);

    assembler.assemble(connection, message, 5);
    assembler.assemble(connection, message + 5, 5);
    assembler.assemble(connection, message + 10, 5);
    assembler.assemble(connection, message + 15, message_length - 15);

    EXPECT_STREQ("This is a test message.", parser.message);
}

TEST_F(MessageAssemblerTest, assemble_multiple) {
    char *message1;
    int message1_length;
    generate_message("This is message 1", &message1_length, &message1);

    char *message2;
    int message2_length;
    generate_message("Message 2", &message2_length, &message2);

    char *combined = new char[message1_length + message2_length];
    memcpy(combined, message1, message1_length);
    memcpy(combined + message1_length, message2, message2_length);
    int combined_length = message1_length + message2_length;
    delete message1;
    delete message2;

    assembler.assemble(connection, combined, combined_length);

    EXPECT_STREQ("Message 2", parser.message);

    delete combined;
}

TEST_F(MessageAssemblerTest, splitted_header) {
    char *message;
    int message_length;
    generate_message("Splitted header message.", &message_length, &message);

    assembler.assemble(connection, message, 1);
    assembler.assemble(connection, message + 1, message_length - 1);

    EXPECT_STREQ("Splitted header message.", parser.message);

    delete message;
}

TEST_F(MessageAssemblerTest, splitted_header_2) {
    char *message;
    int message_length;
    generate_message("Another splitted header message.", &message_length, &message);

    assembler.assemble(connection, message, 1);
    assembler.assemble(connection, message + 1, message_length - 1);

    EXPECT_STREQ("Another splitted header message.", parser.message);

    delete message;
}


} // namespace sprawlnet
