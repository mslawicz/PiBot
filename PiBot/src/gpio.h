/*
 * gpio.h
 *
 *  Created on: 5 sty 2019
 *      Author: Marcin
 */

#ifndef SRC_GPIO_H_
#define SRC_GPIO_H_

#include <pigpio.h>

class GPIO
{
public:
	static void Initialize(void);
	static void Terminate(void);
	GPIO(unsigned gpio_number, unsigned mode, unsigned pull = PI_PUD_OFF);
	int Read(void) {return gpioRead(m_gpio);}
	void Write(unsigned level) {gpioWrite(m_gpio, level);}
	void Toggle(void) {gpioWrite(m_gpio, gpioRead(m_gpio) ^ 0x01);}
private:
	unsigned m_gpio;
};



#endif /* SRC_GPIO_H_ */
