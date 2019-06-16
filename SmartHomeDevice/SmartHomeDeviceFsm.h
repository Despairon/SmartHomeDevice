#pragma once

#include <iostream>

#include "StateMachine.hpp"
#include "EventSystem.h"

namespace SmartHomeDevice_n
{
    using namespace FSM;
    using namespace EventSystem_n;

    using byte = unsigned char;

    namespace State
    {
        enum Values : byte
        {
            INITIAL,
            NETWORK_SCANNING,
            CONNECTING_TO_WIFI,
            CONNECTING_TO_SERVER,
            CONNECTED
        };
    }

    namespace Events
    {
        enum Values : byte
        {
            START,
            NETWORK_SCAN_RESULTS_READY,
            NETWORK_SCAN_TIMEOUT,
            WIFI_CONNECTED,
            WIFI_CONNECTION_FAILED,
            WIFI_CONNECTION_TIMEOUT,
            SERVER_CONNECTED,
            SERVER_CONNECTION_FAILED,
            SERVER_CONNECTION_TIMEOUT,
            DISCONNECTED
        };
    };

    struct EventData
    {
        EventSystem *sender;

        union
        {
            // TODO: fill event data
        } data;
    };

    class SmartHomeDeviceFsm : public StateMachine<State::Values, EventId, EventData, std::ostream>, public EventSubscriber
    {
    public:
        explicit SmartHomeDeviceFsm(const State::Values&);

        std::string stateToString(const State::Values&) const override;
        std::string eventToString(const EventId&) const override;

        void onEvent(EventSystem*, const Event&) override;
    };
}