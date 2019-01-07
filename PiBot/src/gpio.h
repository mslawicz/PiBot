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
	static void initialize(void);
	static void terminate(void);
	GPIO(unsigned gpio_number, unsigned mode, unsigned pull = PI_PUD_OFF);
	int read(void) {return gpioRead(gpio_number);}
	void write(unsigned level) {gpioWrite(gpio_number, level);}
	void toggle(void) {gpioWrite(gpio_number, gpioRead(gpio_number) ^ 0x01);}
	void pulse(unsigned length, unsigned level) {gpioTrigger(gpio_number, length, level);}
private:
	unsigned gpio_number;
};



#endif /* SRC_GPIO_H_ */
