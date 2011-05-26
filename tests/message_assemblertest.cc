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

class TestMessageAssembler : public MessageAssembler {
    public:
    char *message;
    int message_length;

    TestMessageAssembler()
            : message(NULL), 
              message_length(0) {
    }

    PartialMessage *test_get_partial_message(const Connection &connection) {
        return get_partial_message(connection);
    }

    map<int, shared_ptr<PartialMessage> > &test_get_partial_messages() {
        return partial_messages;
    }

    protected:
    void handle(const char *buffer, size_t buffer_size) {
        message = new char[buffer_size];
        memcpy(message, buffer, buffer_size);
        message_length = buffer_size;
    }
};

TEST(MessageAssembler, close_connection) {
    TestMessageAssembler assembler;
    Connection connection(1);
    PartialMessage *partial = new PartialMessage();
    assembler.test_get_partial_messages()[1] =
            shared_ptr<PartialMessage>(partial);

    EXPECT_EQ(partial, assembler.test_get_partial_message(connection));
    assembler.close_connection(connection);
    EXPECT_TRUE(assembler.test_get_partial_message(connection) == NULL);
}

TEST(MessageAssembler, assemble) {
    TestMessageAssembler assembler;
    Connection connection(0);

    char *message;
    int message_length;
    generate_message("This is a test message.", &message_length, &message);

    assembler.assemble(connection, message, message_length);

    EXPECT_STREQ("This is a test message.", assembler.message);

    delete message;
}

TEST(MessageAssembler, assemble_fragments) {
    TestMessageAssembler assembler;
    Connection connection(0);

    char *message;
    int message_length;
    generate_message("This is a test message.", &message_length, &message);

    assembler.assemble(connection, message, 5);
    assembler.assemble(connection, message + 5, 5);
    assembler.assemble(connection, message + 10, 5);
    assembler.assemble(connection, message + 15, message_length - 15);

    EXPECT_STREQ("This is a test message.", assembler.message);
}

TEST(MessageAssembler, assemble_multiple) {
    TestMessageAssembler assembler;
    Connection connection(0);

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

    EXPECT_STREQ("Message 2", assembler.message);

    delete combined;
}


} // namespace sprawlnet
