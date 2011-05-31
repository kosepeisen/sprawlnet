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
#include "message_parser.h"

using std::cout;
using std::endl;
using std::map;
using std::tr1::shared_ptr;

namespace sprawlnet {

PartialMessage::~PartialMessage() {
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
    if (!partial) {
        new_partial_message(connection);
    }

    assemble_partial(connection, buffer, buffer_size);
}

void MessageAssembler::new_partial_message(const Connection &connection) {
    PartialMessage *partial = new PartialMessage();
    partial_messages[connection.get_fd()] =
            shared_ptr<PartialMessage>(partial);
}

void MessageAssembler::initialize_partial(PartialMessage *partial,
        size_t message_size) {
    partial->message_size = message_size;
    partial->bytes_received = 0;
    if (partial->buffer) {
        delete partial->buffer;
    }
    partial->buffer = new char[message_size];
}

size_t MessageAssembler::assemble_header(const Connection &connection,
        const char *buffer, size_t buffer_size) {
    PartialMessage *partial = get_partial_message(connection);

    size_t bytes_left = sizeof(int) - partial->bytes_received;

    size_t bytes_to_copy = 0;
    if (buffer_size < bytes_left) {
        bytes_to_copy = buffer_size;
    } else {
        bytes_to_copy = bytes_left;
    }

    size_t *message_size_buffer = &partial->message_size +
            partial->bytes_received;
    memcpy(message_size_buffer, buffer, bytes_to_copy);
    partial->bytes_received += bytes_to_copy;

    if (partial->bytes_received == sizeof(int)) {
        initialize_partial(partial, partial->message_size);
    }

    return bytes_to_copy;
}


void MessageAssembler::assemble_partial(const Connection &connection,
        const char *buffer, size_t buffer_size) {
    PartialMessage *partial = get_partial_message(connection);

    if (!partial->buffer) {
        size_t offset = assemble_header(connection, buffer, buffer_size);
        buffer += offset;
        buffer_size -= offset;
    }

    size_t bytes_left = partial->message_size - partial->bytes_received;

    size_t bytes_to_copy = 0;
    if (bytes_left <= buffer_size) {
        bytes_to_copy = bytes_left;
    } else {
        bytes_to_copy = buffer_size;
    }

    memcpy(partial->buffer + partial->bytes_received, buffer, bytes_to_copy);
    partial->bytes_received += bytes_to_copy;

    if (partial->bytes_received == partial->message_size) {
        parser->parse(partial->buffer, partial->message_size);
        partial_messages.erase(connection.get_fd());
        assemble(connection, buffer + bytes_to_copy, buffer_size - bytes_to_copy);
    }
}

PartialMessage *
MessageAssembler::get_partial_message(const Connection &connection) {
    map<int, shared_ptr<PartialMessage> >::const_iterator it =
            partial_messages.find(connection.get_fd());
    if (it == partial_messages.end()) {
        return NULL;
    } else {
        return it->second.get();
    }
}

void MessageAssembler::close_connection(const Connection &connection) {
    partial_messages.erase(connection.get_fd());
}

} // namespace sprawlnet
