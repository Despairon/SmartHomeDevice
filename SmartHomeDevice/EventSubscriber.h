#pragma once

#include "Event.h"

namespace EventSystem_n
{
    class EventSubscriber
    {
    public:
        virtual void onEvent(const Event &event) = 0;
    };
}