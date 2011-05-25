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

class Connection;

// TODO: This class needs to have a MessageHandler of some kind.
class MessageAssembler {
    public:
    MessageAssembler() {};

    void assemble(const Connection &connection, const char *buffer,
            size_t buffer_size);

    /**
     * Call this method when a peer disconnects.
     *
     * This will free up space in partial_messages.
     */
    void closeConnection(const Connection &connection);
    
    protected:
    struct PartialMessage {
        ~PartialMessage();
        int bytes_received;
        int message_size;
        char *buffer;
    };

    std::map<int, std::tr1::shared_ptr<PartialMessage> > partial_messages;

    // TODO: Implement this.
    void handle(const char *buffer, size_t buffer_size);

    void assemble_partial(const Connection &connection, const char *buffer,
            size_t buffer_size);
    PartialMessage *get_partial_message(const Connection &connection);

    private:

    MessageAssembler(const MessageAssembler &);
    MessageAssembler &operator=(const MessageAssembler &);
};

} // namespace sprawlnet

#endif

