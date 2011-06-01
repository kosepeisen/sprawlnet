//
//  main.cpp
//  SprawlClient
//
//  Created by Ole Andre Birkedal on 5/29/11.
//  Copyright 2011 Kosepeisen. All rights reserved.
//

#include <string.h>

#include <iostream>

#include "SprawlNet.h"

void CreateMessage(std::string Content, char **MessageDest, size_t *LengthDest) {
    *MessageDest = new char[Content.length() + sizeof(int)];
    *LengthDest = Content.length();
    memcpy(*MessageDest, LengthDest, sizeof(int));
    memcpy(*MessageDest + sizeof(int), Content.c_str(), Content.length());
}

void SendMessage(const SprawlNet *Network, std::string Message) {
    char *MessageBuffer;
    size_t Length;

    CreateMessage(Message, &MessageBuffer, &Length);
    Network->SendData( MessageBuffer, Length );
    delete MessageBuffer;
}

int main (int argc, const char * argv[])
{
    SprawlNet *Network = new SprawlNet( "127.0.0.1", "1337");
    
    if( Network->IsConnected() ) {
        SendMessage(Network, "lalalal1337");
        SendMessage(Network, "SprawlNet loading...");
        SendMessage(Network, "SprawlNet loading....");
        SendMessage(Network, "SprawlNet loading.....");
    }
    
    delete Network;

    return 0;
}

