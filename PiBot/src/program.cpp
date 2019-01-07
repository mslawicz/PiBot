/*
 * program.cpp
 *
 *  Created on: 7 sty 2019
 *      Author: Marcin
 */

#include "program.h"
#include "gpio.h"
#include <iostream>

Program& Program::getInstance(void)
{
	// instantiated on the first use and guaranteed to be destroyed
	static Program instance;
	return instance;
}

Program::~Program()
{
	GPIO::terminate();
	std::cout << "Are you sure it works?" << std::endl;
}

void Program::initialize(void)
{
	GPIO::initialize();
}

void Program::terminate(void)
{

}
