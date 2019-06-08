#include "SmartHomeDevice.h"

namespace SmartHomeDevice_n
{
    SmartHomeDevice::SmartHomeDevice()
    : stateMachine((State)0)
    {
        EventSystem_n::EventSystem::getInstance().subscribe(0, this);
        EventSystem_n::EventSystem::getInstance().subscribe(1, this);
    }

    void SmartHomeDevice::go()
    {
        // TODO: not implemented
    }

    void SmartHomeDevice::onEvent(const EventSystem_n::Event &event)
    {
        // TODO: not implemented
    }
}