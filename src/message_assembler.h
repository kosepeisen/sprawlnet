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
#ifndef MESSAGE_ASSEMBLER_H
#define MESSAGE_ASSEMBLER_H

#include <map>
#include <tr1/memory>

namespace sprawlnet {

class MessageParserInterface;
class Connection;

struct PartialMessage {
    PartialMessage() : bytes_received(0), message_size(0), buffer(NULL) {}
    ~PartialMessage();
    size_t bytes_received;
    size_t message_size;
    char *buffer;
};

class MessageAssembler {
public:
    MessageAssembler(MessageParserInterface * const parser)
            : parser(parser) {};
    virtual ~MessageAssembler() {};

    void assemble(const Connection &connection, const char *buffer,
            size_t buffer_size);

    /** Call this method when a peer disconnects.
     *
     * This will free up space in partial_messages.
     */
    void close_connection(const Connection &connection);
    
protected:
    MessageParserInterface *const parser;
    std::map<int, std::tr1::shared_ptr<PartialMessage> > partial_messages;

    PartialMessage *get_partial_message(const Connection &connection) const;

    /** Register a new partial message for connection. */
    void new_partial_message(const Connection &connection);
    void assemble_partial(const Connection &connection, const char *buffer,
            size_t buffer_size);
    size_t assemble_header(const Connection &connection, const char *buffer,
            size_t buffer_size);

    /** Initialize partial.
     *
     * Allocate the buffer with respect to message_length and reset
     * bytes_received.
     */
    void initialize_partial(PartialMessage *partial,
            size_t message_size) const;

private:
    MessageAssembler(const MessageAssembler &);
    MessageAssembler &operator=(const MessageAssembler &);
};

} // namespace sprawlnet

#endif

