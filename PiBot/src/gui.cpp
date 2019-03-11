/*
 * gui.cpp
 *
 *  Created on: 6 mar 2019
 *      Author: Marcin
 */

#include "gui.h"
#include "program.h"
#include "logger.h"


GUI::GUI()
{
    timeout = std::chrono::hours(10000);
    //timeout = std::chrono::duration_values<std::chrono::milliseconds>::max();
    exitHandler = false;
    pGuiHandlerThread = new std::thread(&GUI::handler, this);
}

GUI::~GUI()
{
    exitHandler = true;
    guiEvent.notify_one();
    pGuiHandlerThread->join();
    delete pGuiHandlerThread;
}

/*
 * GUI handler to be launched in a new thread
 */
void GUI::handler(void)
{
    Logger::getInstance().logEvent(INFO, "GUI handler started");
    do
    {
        //test menu
        std::vector<ScreenMenuItem> testMenuItems =
        {
                ScreenMenuItem("the line #1"),
                ScreenMenuItem("random number " + std::to_string(rand())),
                ScreenMenuItem("the 3rd and the last line")
        };
        ScreenMenu testMenu = ScreenMenu("Test Menu", testMenuItems);
        testMenu.displayItems();

        Logger::getInstance().logEvent(INFO, "GUI loop entry");
        std::this_thread::yield();
        std::unique_lock<std::mutex> lock(guiHandlerMutex);
        guiEvent.wait_for(lock, timeout,[this]() {return (exitHandler); });
        lock.unlock();
        Logger::getInstance().logEvent(INFO, "GUI loop continue");


    } while (!exitHandler);
    Logger::getInstance().logEvent(INFO, "GUI handler exit");
}

