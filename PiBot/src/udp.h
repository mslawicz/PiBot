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
#include <iostream>
#include <sstream>

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
    template<typename... Args> void sendData(Args... args)
    {
        std::stringstream textStream;
        takeNextArgument(textStream, args...);
        write(socketDescriptor, textStream.str().c_str(), textStream.str().length());
    }
private:
    /*
     * takes next argument of user data and put it to the string stream
     */
    template<typename First, typename... Rest> void takeNextArgument(std::stringstream& textStream, First arg0, Rest... args)
    {
        textStream << arg0;
        takeNextArgument(textStream, args...);
    }
    void takeNextArgument(std::stringstream& textStream) {}
    std::string address;
    int port;
    int socketDescriptor;
};

} /* namespace UDP */

#endif /* SRC_UDP_H_ */
