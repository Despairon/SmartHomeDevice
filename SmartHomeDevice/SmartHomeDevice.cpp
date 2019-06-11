#include "SmartHomeDevice.h"

namespace SmartHomeDevice_n
{
    SmartHomeDevice::SmartHomeDevice()
    : stateMachine(State::INITIAL)
    {
        initEventSystem();

        initStateMachine();

        initTaskManager();
    }

    void SmartHomeDevice::initEventSystem()
    {
        // TODO: not implemented
    }

    void SmartHomeDevice::initStateMachine()
    {
        // TODO: not implemented
    }

    void SmartHomeDevice::initTaskManager()
    {
        taskManager.scheduleTask(this);
        taskManager.scheduleTask(&eventSystem, Priority::HIGH);
    }

    void SmartHomeDevice::init()
    {
        // TODO: not implemented
    }

    void SmartHomeDevice::go()
    {
        // TODO: not implemented
    }

    void SmartHomeDevice::terminate()
    {
        // TODO: not implemented
    }

    void SmartHomeDevice::onEvent(EventSystem *sender, const Event &event)
    {
        // TODO: not implemented
    }
}