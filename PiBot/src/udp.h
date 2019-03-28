/*
 * udp.h
 *
 *  Created on: 21 mar 2019
 *      Author: Marcin
 */

#ifndef SRC_UDP_H_
#define SRC_UDP_H_

#include <netdb.h>
#include <unistd.h>
#include <string>

namespace UDP
{

class Client
{
public:
    Client();
    ~Client();
    int setConnection(std::string clientAddress, int clientPort);
    void disconnect(void);
    /*
     * send user data to UDP server
     */
    void sendData(std::string data) const
    {
        write(socketDescriptor, data.c_str(), data.length());
    }
private:
    std::string address;
    int port;
    int socketDescriptor;
};

class Server
{
public:
    Server();
    ~Server();
    int start(int clientPort);
    void stop(void);
private:
    int port;
    int socketDescriptor;
};

} /* namespace UDP */

#endif /* SRC_UDP_H_ */
