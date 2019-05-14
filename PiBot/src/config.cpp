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

Config::Config()
{
    parameters.emplace("motor_pid_p",
            Actions([this](){Program::getInstance().getRobot()->getPitchPID()->setKp(getFloatFromFile());},
            [this](){cfgFile << Program::getInstance().getRobot()->getPitchPID()->getKp();}));
    parameters.emplace("motor_pid_i",
            Actions([this](){Program::getInstance().getRobot()->getPitchPID()->setKi(getFloatFromFile());},
            [this](){cfgFile << Program::getInstance().getRobot()->getPitchPID()->getKi();}));
    parameters.emplace("motor_pid_d",
            Actions([this](){Program::getInstance().getRobot()->getPitchPID()->setKd(getFloatFromFile());},
            [this](){cfgFile << Program::getInstance().getRobot()->getPitchPID()->getKd();}));
    parameters.emplace("speed_pid_p",
            Actions([this](){Program::getInstance().getRobot()->getSpeedPID()->setKp(getFloatFromFile());},
            [this](){cfgFile << Program::getInstance().getRobot()->getSpeedPID()->getKp();}));
    parameters.emplace("speed_pid_i",
            Actions([this](){Program::getInstance().getRobot()->getSpeedPID()->setKi(getFloatFromFile());},
            [this](){cfgFile << Program::getInstance().getRobot()->getSpeedPID()->getKi();}));
    parameters.emplace("speed_pid_d",
            Actions([this](){Program::getInstance().getRobot()->getSpeedPID()->setKd(getFloatFromFile());},
            [this](){cfgFile << Program::getInstance().getRobot()->getSpeedPID()->getKd();}));
    parameters.emplace("alpha",
            Actions([this](){Program::getInstance().getRobot()->setAlpha(getFloatFromFile());},
            [this](){cfgFile << Program::getInstance().getRobot()->getAlpha();}));
    parameters.emplace("target_pitch",
            Actions([this](){Program::getInstance().getRobot()->setTargetPitch(getFloatFromFile());},
            [this](){cfgFile << Program::getInstance().getRobot()->getTargetPitch();}));

    cfgFile.open(CfgFileName, std::ios::in);
        if(cfgFile.is_open())
        {
            // load parameters from cfg file
            std::string parameterDesignator;
            while(cfgFile >> parameterDesignator)
            {
                if(parameters.find(parameterDesignator) != parameters.end())
                {
                    // known parameter
                    parameters.find(parameterDesignator)->second.first();
                }
                else
                {
                    Logger::getInstance().logEvent(WARNING, "unknown configuration parameter: ", parameterDesignator);
                    cfgFile.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                }
            }

            cfgFile.close();
            Logger::getInstance().logEvent(INFO, "configuration file read");
        }
        else
        {
            Logger::getInstance().logEvent(ERROR, "unable to open configuration file");
        }
}

Config::~Config()
{
    cfgFile.open(CfgFileName, std::ios::out);
    if(cfgFile.is_open())
    {
        // save parameters to cfg file
        for(auto parameter : parameters)
        {
            cfgFile << parameter.first << " ";
            parameter.second.second();
            cfgFile << "\n";
        }

        cfgFile.close();
        Logger::getInstance().logEvent(INFO, "configuration file saved");
    }
    else
    {
        Logger::getInstance().logEvent(ERROR, "unable to create configuration file");
    }
}

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
