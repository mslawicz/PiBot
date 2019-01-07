/*
 * program.cpp
 *
 *  Created on: 7 sty 2019
 *      Author: Marcin
 */

#include "program.h"
#include "gpio.h"

Program* Program::pointer_to_object = nullptr;

Program* Program::instance(void)
{
	if(pointer_to_object == nullptr)
	{
		pointer_to_object = new Program();
	}
	return pointer_to_object;
}

void Program::initialize(void)
{
	GPIO::initialize();
}

void Program::terminate(void)
{
	GPIO::terminate();
}
