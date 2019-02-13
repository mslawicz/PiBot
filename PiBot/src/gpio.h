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
    GYRO_INT = 4,   //TODO change to another one
    SW1 = 5,
    SW2 = 24,
    SW3 = 22,
    SW4 = 23,
    SW5 = 4,
    SW6 = 17,
    BACKLIGHT = 27,
    IR_RECEIVER = 26
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

class PushButton : public GPIO
{
public:
    PushButton(GpioPin gpioNumber, uint32_t debounceTime = 20000);
    bool hasBeenPressed(void);
private:
    void stateMachine(void);
    uint32_t debounce_time;
    uint8_t state;
    uint32_t eventTime;
    bool keyIsPressed;
    bool keyHasBeenPressed;
};

#endif /* SRC_GPIO_H_ */
