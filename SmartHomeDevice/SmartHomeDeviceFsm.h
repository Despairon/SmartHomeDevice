#pragma once

#include <iostream>

#include "StateMachine.hpp"
#include "EventSystem.h"

namespace SmartHomeDevice_n
{
    using namespace FSM;
    using namespace EventSystem_n;

    using byte = unsigned char;

    enum State : byte
    {
        INITIAL
    };

    struct EventData
    {
        EventSystem *sender;

        union
        {
            // TODO: fill event data
        } data;
    };

    class SmartHomeDeviceFsm : public StateMachine<State, EventId, EventData, std::ostream>, public EventSubscriber
    {
    public:
        explicit SmartHomeDeviceFsm(const State&);

        std::string stateToString(const State&) const override;
        std::string eventToString(const EventId&) const override;

        void onEvent(EventSystem*, const Event&) override;
    };
}