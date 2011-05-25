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

#include "connection.h"
#include "message_assembler.h"

using std::cout;
using std::endl;
using std::map;
using std::tr1::shared_ptr;

namespace sprawlnet {

MessageAssembler::PartialMessage::~PartialMessage() {
    delete buffer;
}

void MessageAssembler::assemble(const Connection &connection,
        const char *buffer, size_t buffer_size) {
    if (buffer_size == 0) {
        return;
    } else if (buffer_size < 0) {
        cout << "MessageAssembler: Buffer size < 0." << endl;
        return;
    }

    PartialMessage *partial = get_partial_message(connection);

    if (partial) {
        assemble_partial(connection, buffer, buffer_size);
    } else {
        if (buffer_size < sizeof(int)) {
            // TODO: Make a partial message; handle split message headers.
            cout << "Handling message. Buffer too small." << endl;
            return;
        }

        // TODO: Add a maximum size and error handling if a message will be too
        // big.
        partial = new PartialMessage();
        partial->bytes_received = 0;
        partial->message_size = *(int*)buffer;
        partial->buffer = new char[partial->message_size];

        partial_messages[connection.get_fd()] =
                shared_ptr<PartialMessage>(partial);
        assemble_partial(connection, buffer + sizeof(int),
                buffer_size - sizeof(int));
    }
}

void MessageAssembler::assemble_partial(const Connection &connection,
        const char *buffer, size_t buffer_size) {
    PartialMessage *partial = get_partial_message(connection);

    size_t bytes_left = partial->message_size - partial->bytes_received;

    size_t bytes_to_copy = 0;
    if (bytes_left <= buffer_size) {
        bytes_to_copy = bytes_left;
    } else {
        bytes_to_copy = buffer_size;
    }

    memcpy(partial->buffer + partial->bytes_received, buffer, bytes_to_copy);
    partial->bytes_received += bytes_to_copy;

    handle(partial->buffer, partial->message_size);
    partial_messages.erase(connection.get_fd());

    assemble(connection, buffer + bytes_to_copy, buffer_size - bytes_to_copy);
}

MessageAssembler::PartialMessage *
MessageAssembler::get_partial_message(const Connection &connection) {
    map<int, shared_ptr<PartialMessage> >::const_iterator it =
            partial_messages.find(connection.get_fd());
    if (it == partial_messages.end()) {
        return NULL;
    } else {
        return it->second.get();
    }
}

void MessageAssembler::handle(const char *buffer, size_t buffer_size) {
    char *message = new char[buffer_size+1];
    memcpy(message, buffer, buffer_size);
    message[buffer_size] = '\0';
    cout << "Got message: " << message << endl;

    delete message;
}

void MessageAssembler::closeConnection(const Connection &connection) {
    partial_messages.erase(connection.get_fd());
}

} // namespace sprawlnet
