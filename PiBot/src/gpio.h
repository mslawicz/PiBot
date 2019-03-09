/*
 * gpio.h
 *
 *  Created on: 5 sty 2019
 *      Author: Marcin
 */

#ifndef SRC_GPIO_H_
#define SRC_GPIO_H_

#include <pigpio.h>

enum GpioPin
{
       //TODO change to another one
    SW5 = 4,
    SW1 = 5,
    SW6 = 17,
    GYRO_INT = 18,
    POWER_OFF = 21,
    SW3 = 22,
    SW4 = 23,
    SW2 = 24,
    ILI9341_CD = 25,
    IR_RECEIVER = 26,
    BACKLIGHT = 27
};

class GPIO
{
public:
	static void initialize(void);
	static void terminate(void);
	GPIO(GpioPin gpioNumber, unsigned mode, unsigned pull = PI_PUD_OFF);
	int read(void) const {return gpioRead(gpio_number);}
	void write(unsigned level) {gpioWrite(gpio_number, level);}
	void toggle(void) {gpioWrite(gpio_number, gpioRead(gpio_number) ^ 0x01);}
	void pulse(unsigned length, unsigned level) const {gpioTrigger(gpio_number, length, level);}
	unsigned int getNumber(void) const { return gpio_number; }
private:
	unsigned gpio_number;
};

#endif /* SRC_GPIO_H_ */
