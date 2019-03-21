/*
 * udp.h
 *
 *  Created on: 21 mar 2019
 *      Author: Marcin
 */

#ifndef SRC_UDP_H_
#define SRC_UDP_H_

#include <netdb.h>
#include <iostream>

namespace UDP
{

class Client
{
public:
    Client(std::string clientAddress, int clientPort);
    ~Client();
    int Connect(void);
private:
    std::string address;
    int port;
    int socketDescriptor;
};

} /* namespace UDP */

#endif /* SRC_UDP_H_ */
