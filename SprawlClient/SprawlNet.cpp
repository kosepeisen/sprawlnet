//
//  SprawlNet.cpp
//  SprawlClient
//
//  Created by Ole Andre Birkedal on 5/29/11.
//  Copyright 2011 Kosepeisen. All rights reserved.
//

#include <string.h>

#include "SprawlNet.h"

SprawlNet::SprawlNet( const std::string &Host, const std::string &Port )
{
    Connected = false;
    Socket = 0;

    Connect( Host, Port );
}

SprawlNet::~SprawlNet()
{
    close( Socket );
}

size_t SprawlNet::SendData( const void *Data, size_t Length ) const
{
    size_t BytesSent = send( Socket, Data, Length, 0 ); // No flags
    
    if( BytesSent == -1 )
        std::cerr << "Could not write data:" << strerror( errno) << std::endl;
    
    return BytesSent;
}

int SprawlNet::Connect( const std::string &Host, const std::string &Port)
{
    struct addrinfo hints, *res0, *res;
    int Error;
    bzero( &hints, sizeof(hints) );
    hints.ai_family   = AF_UNSPEC; // ipv4 or ipv4
    hints.ai_socktype = SOCK_STREAM; // TCP

    Error = getaddrinfo( Host.c_str(), Port.c_str(), &hints, &res0 );
    if( Error )
    {
        std::cerr << "Could not resolve host: " << gai_strerror( Error ) << std::endl;
    }

    for ( res = res0; res != NULL; res = res->ai_next )
    {
        Socket = socket( res->ai_family, res->ai_socktype, res->ai_protocol );
        if( Socket == -1 )
        {
            std::cerr << "Could not create socket: " << strerror( errno ) << std::endl;
            continue;
        }

        Error = connect( Socket, res->ai_addr, res->ai_addrlen );
        if( Error == -1)
        {
            std::cerr << "Connection failed: " << strerror( errno ) << std::endl;
            close( Socket );
            continue;
        }

        break;
    }

    if( res == NULL )
    {
        std::cerr << "Could not connect to " << Host << ":" << Port << std::endl;
        return 1;
    }

    freeaddrinfo( res0 );
    Connected = true;

    return 0;
}
