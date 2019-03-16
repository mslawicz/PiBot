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

typedef std::map<std::string, std::pair<std::string, std::string>> LanIfMap;

class Config
{
public:
    LanIfMap getIpAddresses(void);
private:
    int updateLanInterfaces(void);
    LanIfMap lanInterfaces;
};

#endif /* SRC_CONFIG_H_ */
