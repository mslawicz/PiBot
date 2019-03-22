/*
 * udp.cpp
 *
 *  Created on: 21 mar 2019
 *      Author: Marcin
 */

#include "udp.h"
#include "logger.h"
#include <sys/socket.h>
#include <cstring>

namespace UDP
{

Client::Client()
{
    port = 0;
    socketDescriptor = -1;
}

Client::~Client()
{
    if(socketDescriptor != -1)
    {
        close(socketDescriptor);
        Logger::getInstance().logEvent(INFO, "Disconnected from UDP server ", address.c_str(), ":", std::to_string(port).c_str());
    }
}

/*
 * set connection to UDP server
 */
int Client::setConnection(std::string clientAddress, int clientPort)
{
    address = clientAddress;
    port = clientPort;
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

        if(connect(socketDescriptor, pItem->ai_addr, pItem->ai_addrlen) == 0)
        {
            // connected to server
            Logger::getInstance().logEvent(INFO, "Connected to UDP server ", address.c_str(), ":", std::to_string(port).c_str());
            break;
        }

        close(socketDescriptor);
    }

    if(pItem == nullptr)
    {
        // no connection
        Logger::getInstance().logEvent(ERROR, "Failed to connect to UDP server ", address.c_str(), ":", std::to_string(port).c_str());
        return -1;
    }

    freeaddrinfo(pAddrInfo);

    return 0;
}


} /* namespace UDP */
