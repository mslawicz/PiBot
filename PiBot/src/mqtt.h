/*
 * mqtt.h
 *
 *  Created on: 2 mar 2019
 *      Author: Marcin
 */

#ifndef SRC_MQTT_H_
#define SRC_MQTT_H_

#include <mosquittopp.h>
#include <iostream>

class MQTT : public mosqpp::mosquittopp
{
public:
    MQTT(std::string id, std::string hostAddress);
    ~MQTT();
private:
    void on_connect(int rc) override;
    void on_disconnect(int rc) override;
    void on_subscribe(int mid, int qos_count, const int *granted_qos) override;
    std::string address;
    int port;
};

#endif /* SRC_MQTT_H_ */
