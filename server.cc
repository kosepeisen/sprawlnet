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
#include <memory>

#include "message_assembler.h"
#include "message_parser.h"
#include "socket_server.h"

using std::auto_ptr;
using sprawlnet::MessageAssembler;
using sprawlnet::MessageParser;
using sprawlnet::SocketServer;

int main() {
    MessageParser parser;
    MessageAssembler assembler(&parser);
    auto_ptr<SocketServer> server(SocketServer::create(&assembler));
    server->bind("1337");
    server->listen();
    server->destroy();
    return 0;
}
