/*
 * console.h
 *
 *  Created on: 13 mar 2019
 *      Author: Marcin
 */

#ifndef SRC_CONSOLE_H_
#define SRC_CONSOLE_H_

#include <thread>

class Console
{
public:
    Console();
    ~Console();
private:
    void handler(void);
    std::thread* pConsoleHandlerThread;
};

#endif /* SRC_CONSOLE_H_ */
