/*
 * gui.h
 *
 *  Created on: 6 mar 2019
 *      Author: Marcin
 */

#ifndef SRC_GUI_H_
#define SRC_GUI_H_

#include <mutex>
#include <condition_variable>
#include <chrono>
#include <thread>

class GUI
{
public:
    GUI();
    ~GUI();
private:
    void handler(void);
    std::mutex guiHandlerMutex;
    std::condition_variable guiEvent;
    std::chrono::milliseconds timeout;
    bool exitHandler;
    std::thread* pGuiHandlerThread;
};

#endif /* SRC_GUI_H_ */
