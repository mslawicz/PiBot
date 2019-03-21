/*
 * udp.cpp
 *
 *  Created on: 21 mar 2019
 *      Author: Marcin
 */

#include "udp.h"
#include <sys/socket.h>
#include <unistd.h>
#include <cstring>

namespace UDP
{

Client::Client(std::string clientAddress, int clientPort)
    : address(clientAddress)
    , port(clientPort)
{
    socketDescriptor = -1;

}

Client::~Client()
{
    // TODO Auto-generated destructor stub
}

int Client::Connect(void)
{
    struct addrinfo hints;
    struct addrinfo* pAddrInfo;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_protocol = IPPROTO_UDP;
    hints.ai_flags = 0;
    int error = getaddrinfo(address.c_str(), std::to_string(port).c_str(), &hints, &pAddrInfo);
    if(error)
    {
        return error;
    }

    struct addrinfo* pItem;
    for(pItem = pAddrInfo; pItem != nullptr; pItem = pItem->ai_next)
    {
        socketDescriptor = socket(pItem->ai_family,  pItem->ai_socktype, pItem->ai_protocol);

        if(socketDescriptor == -1)
        {
            continue;
        }

        if(connect(socketDescriptor, pItem->ai_addr, pItem->ai_addrlen))
        {
            // connected to server
            break;
        }

        close(socketDescriptor);
    }

    if(pItem == nullptr)
    {
        // no connection
        return -1;
    }

    freeaddrinfo(pAddrInfo);

    return 0;
}


} /* namespace UDP */
