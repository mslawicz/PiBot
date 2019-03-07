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
    // enable pushbutton interrupts
    gpioGlitchFilter(GpioPin::SW1, 20000);
    gpioSetAlertFuncEx(GpioPin::SW1, GUI::pushbuttonInterruptCallback, this);
}

GUI::~GUI()
{
    //disable pushbutton interrupts
    gpioSetAlertFuncEx(GpioPin::SW1, nullptr, this);
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
        Logger::getInstance().logEvent(INFO, "GUI loop entry");
        std::this_thread::yield();
        std::unique_lock<std::mutex> lock(guiHandlerMutex);
        guiEvent.wait_for(lock, timeout,[this]() {return (exitHandler); });
        lock.unlock();
        Logger::getInstance().logEvent(INFO, "GUI loop continue");


    } while (!exitHandler);
    Logger::getInstance().logEvent(INFO, "GUI handler exit");
}

/*
 * callback function for pushbutton generated interrupts
 * it must be a static method, but the pointer to drive object is passed as an argument
 * Parameter   Value    Meaning
 *
 * gpio        0-53     The GPIO which has changed state
 *
 * level       0-2      0 = change to low (a falling edge)
 *                      1 = change to high (a rising edge)
 *                      2 = no level change (interrupt timeout)
 *
 * tick        32 bit   The number of microseconds since boot
 *                      WARNING: this wraps around from
 *                      4294967295 to 0 roughly every 72 minutes
 *
 * pDriveObject pointer Pointer to an arbitrary object
 *
 */
void GUI::pushbuttonInterruptCallback(int gpio, int level, uint32_t tick,
        void* pGuiObject)
{
    static_cast<GUI*>(pGuiObject)->pushbuttonService(gpio, level, tick);
}

/*
 * pushbutton service
*/
void GUI::pushbuttonService(int gpio, int level, uint32_t tick)
{
    Logger::getInstance().logEvent(INFO, "pushbutton pressed: ", gpio, ", level=", level);
}
