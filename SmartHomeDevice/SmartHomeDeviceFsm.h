#pragma once

#include "StateMachine.hpp"
#include "EventSystem.h"
#include "DebugDevice.h"

namespace SmartHomeDevice_n
{
    #define MAX_SSID_LENGTH 64
    #define MAX_ERROR_LENGTH 128
    #define MAX_HOSTNAME_LENGTH 64
    #define MAX_SERVER_RESPONSE_LEN 256
    
    using namespace FSM;
    using namespace EventSystem_n;
    using namespace DebugDevice_n;

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
            NETWORK_SCAN_FAILED,
            NETWORK_SCAN_TIMEOUT,
            NETWORK_PICKED,
            WIFI_CONNECTED,
            WIFI_CONNECTION_FAILED,
            WIFI_CONNECTION_RETRIES_EXHAUSTED,
            WIFI_CONNECTION_TIMEOUT,
            SERVER_CONNECTED,
            SERVER_PICKED,
            SERVER_CONNECTION_FAILED,
            SERVER_CONNECTION_RETRIES_EXHAUSTED,
            SERVER_CONNECTION_TIMEOUT,
            DEVICE_STATUS_REQUEST_TIMEOUT,
            DATA_AVAILABLE,
            DEVICE_ID_RECEIVED,
            DEVICE_ID_ERROR,
            DISCONNECTED,
            TIMER_EXPIRED,
            FATAL_ERROR
        };
    };

    struct NetworkInfo
    {
        char ssid[MAX_SSID_LENGTH];
        int  channel;
        int  rssi;
        bool isOpen;
    };

    struct HostInfo
    {
        char host[MAX_HOSTNAME_LENGTH];
        unsigned short port;
    };

    struct EventData
    {
        EventSystem *sender;

        union
        {
            NetworkInfo networkInfo;
            HostInfo hostInfo;
            char errorStr[MAX_ERROR_LENGTH];
            char serverResponseStr[MAX_SERVER_RESPONSE_LEN];

        } data;
    };

    class SmartHomeDeviceFsm : public StateMachine<State::Values, EventId, EventData, DebugDevice>, public EventSubscriber
    {
    private:
        DebugDevice *debugDevice;

    public:
        explicit SmartHomeDeviceFsm(const State::Values&);

        std::string stateToString(const State::Values&) const override;
        std::string eventToString(const EventId&) const override;

        void onEvent(EventSystem*, const Event&) override;

        void setDebugDevice(DebugDevice*);
    };
}