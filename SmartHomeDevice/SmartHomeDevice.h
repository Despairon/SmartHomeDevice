#pragma once

#include "HttpMessage.h"
#include "SmartHomeDeviceFsm.h"
#include "EventSystem.h"

namespace SmartHomeDevice_n
{
    class SmartHomeDevice : EventSystem_n::EventSubscriber 
    {
    private:
        SmartHomeDeviceFsm stateMachine;

    protected:

    public:
        SmartHomeDevice();

        void go();
        void onEvent(const EventSystem_n::Event&) override;
    };
}