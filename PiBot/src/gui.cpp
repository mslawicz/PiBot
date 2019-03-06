/*
 * gui.cpp
 *
 *  Created on: 6 mar 2019
 *      Author: Marcin
 */

#include "gui.h"
#include "program.h"
#include "logger.h"
#include <thread>


GUI::GUI()
{
    exitHandler = false;

}

GUI::~GUI()
{
    // TODO Auto-generated destructor stub
}

/*
 * GUI handler to be launched in a new thread
 */
void GUI::handler(void)
{
    Logger::getInstance().logEvent(INFO, "GUI handler started");
    do
    {
        std::this_thread::yield();
        std::unique_lock<std::mutex> lock(guiHandlerMutex);
        guiEvent.wait_for(lock, timeout,[this]() {return (exitHandler); });
        lock.unlock();



    } while (!exitHandler);
}
