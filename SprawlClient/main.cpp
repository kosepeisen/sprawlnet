//
//  main.cpp
//  SprawlClient
//
//  Created by Ole Andre Birkedal on 5/29/11.
//  Copyright 2011 Kosepeisen. All rights reserved.
//

#include <iostream>

#include "SprawlNet.h"

int main (int argc, const char * argv[])
{
    SprawlNet *Network = new SprawlNet( "127.0.0.1", "1337");
    
    std::string test("lalal1337");
    
    if( Network->IsConnected() )
        Network->SendData( test.c_str(), test.length() );
    
    delete Network;

    return 0;
}

