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
    disconnect();
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
        Logger::getInstance().logEvent(ERROR, "Cannot get address info of host ", address.c_str(), ":", std::to_string(port).c_str());
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

/*
 * clear connection
 */
void Client::disconnect(void)
{
    if(socketDescriptor != -1)
    {
        close(socketDescriptor);
        Logger::getInstance().logEvent(INFO, "Disconnected from UDP server ", address.c_str(), ":", std::to_string(port).c_str());
        socketDescriptor = -1;
        port = 0;
    }
}

Server::Server()
{
    port = 0;
    socketDescriptor = -1;
    terminateThread = false;
    pServerHandlerThread = nullptr;
}

Server::~Server()
{

}

int Server::start(int clientPort)
{
    port = clientPort;
    struct addrinfo hints;
    struct addrinfo* pAddrInfo;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;    // IP4 or IP6
    hints.ai_socktype = SOCK_DGRAM; // datagram (UDP)
    hints.ai_protocol = 0;  // any
    hints.ai_flags = AI_PASSIVE;    // for wildcard IP address
    hints.ai_canonname = nullptr;
    hints.ai_addr = nullptr;
    hints.ai_next = nullptr;
    int error = getaddrinfo(nullptr, std::to_string(port).c_str(), &hints, &pAddrInfo);
    if(error)
    {
        Logger::getInstance().logEvent(ERROR, "Cannot get UDP server address info");
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

        if(bind(socketDescriptor, pItem->ai_addr, pItem->ai_addrlen) == 0)
        {
            // success on bind
            Logger::getInstance().logEvent(INFO, "UDP server socket bound using port ", std::to_string(port).c_str());
            break;
        }

        close(socketDescriptor);
    }

    if(pItem == nullptr)
    {
        // no address succeeded
        Logger::getInstance().logEvent(ERROR, "Could not bind UDP server socket");
        return -1;
    }

    freeaddrinfo(pAddrInfo);

    terminateThread = false;
    pServerHandlerThread =  new std::thread(&Server::handler, this);

    return 0;
}

void Server::stop(void)
{
    if(socketDescriptor != -1)
    {
        terminateThread = true;
        shutdown(socketDescriptor, SHUT_RDWR);
        Logger::getInstance().logEvent(INFO, "UDP server socket unbound");
        socketDescriptor = -1;
        port = 0;
        pServerHandlerThread->join();
        delete pServerHandlerThread;
    }
}

/*
 * Server handler to be launched in a new thread
 */
void Server::handler(void)
{
    Logger::getInstance().logEvent(INFO, "UDP server handler started");
    const size_t BufferSize = 200;
    char buffer[BufferSize];
    struct sockaddr_in remoteAddress;
    socklen_t remoteAddressLength = sizeof(struct sockaddr_in);

    do
    {
        std::this_thread::yield();
        recvfrom(socketDescriptor, buffer, BufferSize, 0, (struct sockaddr *)&remoteAddress, &remoteAddressLength);
    } while (!terminateThread);

    Logger::getInstance().logEvent(INFO, "UDP server handler exit");
}

} /* namespace UDP */
