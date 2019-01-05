/*
 * exit_codes.h
 *
 *  Created on: 5 sty 2019
 *      Author: Marcin
 */

#ifndef SRC_EXIT_CODES_H_
#define SRC_EXIT_CODES_H_

enum ExitCode
{
	OK,
	GENERAL_ERROR,
	SHELL_ERROR,
	GPIO_INITIALIZATION_ERROR,
	BAD_GPIO_NUMBER,
	BAD_GPIO_MODE
};


#endif /* SRC_EXIT_CODES_H_ */
