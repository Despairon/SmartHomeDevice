#pragma once

#include "StateMachine.hpp"

namespace SmartHomeDevice_n
{
    using namespace FSM;
    
    using byte = unsigned char;

    enum State : byte
    {

    };

    enum Event : byte
    {

    };

    struct EventData
    {

    };

    class SmartHomeDeviceFsm : public StateMachine<State, Event, EventData, int>
    {
    public:
        explicit SmartHomeDeviceFsm(const State&);

        std::string stateToString(const State&) const override;
        std::string eventToString(const Event&) const override;
    };
}