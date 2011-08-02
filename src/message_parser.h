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
#ifndef MESSAGE_PARSER_H
#define MESSAGE_PARSER_H

#include "message_parser_interface.h"

#include <cstring>

#include "message/message_header.pb.h"

namespace sprawlnet {

/**
 * Parser a message header and sends it to a MessageHandler.
 *
 * Messages must be at least 4 bytes long.
 */
class MessageParser : public MessageParserInterface {
    friend class MessageParserTest;
public:
    MessageParser() {}

    void parse(const char *message, size_t message_size);

private:
    bool is_valid_message(const char *message, size_t message_size);
    size_t get_header_length(const char* message);

    /** Parse message header.
     *
     * Returns true if header was parsed successfully, false otherwise.
     */
    bool parse_header(const char* message, size_t header_length,
            message::MessageHeader* result);

    MessageParser(const MessageParser &);
    MessageParser &operator=(const MessageParser &);
};

} // namespace sprawlnet

#endif


