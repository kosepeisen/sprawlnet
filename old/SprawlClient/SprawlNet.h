//
//  SprawlNet.h
//  SprawlClient
//
//  Created by Ole Andre Birkedal on 5/29/11.
//  Copyright 2011 Kosepeisen. All rights reserved.
//

#pragma once

#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <errno.h>
#include <iostream>

class SprawlNet
{
public:
    SprawlNet( const std::string &Host, const std::string &Port );
    ~SprawlNet();
    
    size_t SendData( const void *Data, size_t Length ) const;
    bool IsConnected() { return Connected; }

private:
    int Socket;
    bool Connected;
    int Connect( const std::string &host, const std::string &port );
};