/*
 * config.h
 *
 *  Created on: 16 mar 2019
 *      Author: Marcin
 */

#ifndef SRC_CONFIG_H_
#define SRC_CONFIG_H_

#include <iostream>
#include <map>
#include <utility>
#include <functional>
#include <unordered_map>
#include <map>
#include <fstream>

typedef std::pair<std::function<void(void)>, std::function<void(void)>> Actions;
typedef std::map<std::string, std::pair<std::string, std::string>> LanIfMap;

class Config
{
public:
    Config();
    ~Config();
    LanIfMap getIpAddresses(void);
private:
    int updateLanInterfaces(void);
    float getFloatFromFile(void) { float x; cfgFile >> x; return x; }
    LanIfMap lanInterfaces;
    std::unordered_map<std::string, Actions> parameters;
    std::fstream cfgFile;
    const std::string CfgFileName = "/home/pi/PiBot/PiBot.cfg";
};

#endif /* SRC_CONFIG_H_ */
