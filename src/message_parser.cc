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

#include <cstring>
#include <iostream>

#include "message/message_header.pb.h"

using std::cout;
using std::cerr;
using std::endl;

namespace sprawlnet {

bool MessageParser::is_valid_message(const char *message, size_t message_size) {
    if (message_size < 4) {
        cerr << "Message invalid: message_size < 4" << endl;
        return false;
    }
    return true;
}

void MessageParser::parse(const char *message, size_t message_size) {
    if (!is_valid_message(message, message_size)) {
        cerr << "Ignoring invalid message." << endl;    
        return;
    }
    char *message_ = new char[message_size + 1];
    memcpy(message_, message, message_size);
    message_[message_size] = '\0';

    cout << "Parsing message: " << message_ << endl;
    delete message_;
}

size_t MessageParser::get_header_length(const char *message) {
    size_t header_length;
    memcpy(&header_length, message, sizeof(size_t));
    return header_length;
}

bool MessageParser::parse_header(const char *message, size_t header_length,
        message::MessageHeader* result) {
    return result->ParseFromString(message);
}

} // namespace sprawlnet
