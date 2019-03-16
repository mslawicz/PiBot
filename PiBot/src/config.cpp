/*
 * config.cpp
 *
 *  Created on: 16 mar 2019
 *      Author: Marcin
 */

#include "config.h"
#include "program.h"
#include "logger.h"
#include <ifaddrs.h>
#include <sys/socket.h>
#include <netdb.h>

/*
 * reads IP addresses from the system
 */
int Config::updateLanInterfaces(void)
{
    const int BufferSize = 20;
    char buffer[BufferSize];
    struct ifaddrs* pIfAddrs = nullptr;
    int errorCode;

    // get lan interfaces data
    if((errorCode = getifaddrs(&pIfAddrs)) < 0)
    {
        Logger::getInstance().logEvent(WARNING, "failed to read lan interface data");
        return errorCode;
    }

    // store head of the structure list for later memory release
    struct ifaddrs* pIfAddrHead = pIfAddrs;

    // iterate through all interfaces
    while(pIfAddrs != nullptr)
    {
        if(pIfAddrs->ifa_addr == nullptr)
        {
            //ignore item without interface data
            continue;
        }

        auto family = pIfAddrs->ifa_addr->sa_family;
        if(family == AF_INET)
        {
            // get only IPv4 data
            errorCode = getnameinfo(pIfAddrs->ifa_addr, sizeof(struct sockaddr_in), buffer, BufferSize, NULL, 0, NI_NUMERICHOST);
            if(errorCode)
            {
                Logger::getInstance().logEvent(WARNING, "failed to read lan interface address of ", pIfAddrs->ifa_name);
                continue;
            }
            std::string hostAddress(buffer);

            errorCode = getnameinfo(pIfAddrs->ifa_netmask, sizeof(struct sockaddr_in), buffer, BufferSize, NULL, 0, NI_NUMERICHOST);
            if(errorCode)
            {
                Logger::getInstance().logEvent(WARNING, "failed to read lan interface address mask of ", pIfAddrs->ifa_name);
                continue;
            }
            std::string hostMask(buffer);

            lanInterfaces[pIfAddrs->ifa_name] = std::pair<std::string, std::string>(hostAddress, hostMask);
        }

        pIfAddrs = pIfAddrs->ifa_next;
    }

    freeifaddrs(pIfAddrHead);
    return errorCode;
}

/*
 * get the map of lan interfaces
 */
LanIfMap Config::getIpAddresses(void)
{
    updateLanInterfaces();
    return lanInterfaces;
}
