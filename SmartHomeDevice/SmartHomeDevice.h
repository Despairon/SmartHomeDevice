#pragma once

#include "HttpMessage.h"
#include "SmartHomeDeviceFsm.h"
#include "EventSystem.h"
#include "TaskManager.h"
#include "TimerManager.h"

namespace SmartHomeDevice_n
{
    #define MAX_SSID_LENGTH 128U

    using namespace TaskManager_n;
    using namespace TimerManager_n;

    namespace WifiStatus
    {
        enum Values : byte
        {
            IDLE,
            NO_SSID_AVAILABLE,
            SCAN_COMPLETED,
            CONNECTED,
            CONNECT_FAILED,
            CONNECTION_LOST,
            DISCONNECTED
        };
    };

    struct WifiConfiguration
    {
        std::map<std::string, std::string> knownNetworks;
        std::map<std::string, unsigned short> knownHosts;
        const unsigned short networkScanTimeout;
        const unsigned short wifiConnectionTimeout;
        const unsigned short serverConnectionTimeout;
    };

    struct NetworkInfo
    {
        char ssid[MAX_SSID_LENGTH];
        int  channel;
        int  rssi;
        bool isOpen;
    };

    class SmartHomeDevice : public EventSubscriber, public Task
    {
    private:
        EventSystem         eventSystem;
        SmartHomeDeviceFsm  stateMachine;
        TimerManager       *timerManager;
        TaskManager         taskManager;

        WifiConfiguration   configuration;

        std::map<Events::Values, EventId> events;

        // timers
        TimerHandle networkScanTimer;
        TimerHandle wifiConnectionTimer;
        TimerHandle serverConnectionTimer;

        void initEventSystem();
        void initStateMachine();
        void initTimers();
        void initTaskManager();

        // FSM callbacks
        void startNetworksScan(const EventData&);

    protected:
        // WiFi interface
        virtual void                connectToWiFi(const std::string&, const std::string&) = 0;
        virtual void                disconnectFromWiFi() = 0;
        virtual void                scanForNetworks(std::function<void(int)>) = 0;
        virtual NetworkInfo         getInfoForNetwork(const byte&) = 0;
        virtual void                connectToServer(const std::string&, const unsigned short&) = 0;
        virtual void                disconnectFromServer() = 0;
        virtual bool                dataAvailable() = 0;
        virtual bool                connectedToServer() = 0;
        virtual std::string         readData() = 0;
        virtual void                sendData(const std::string&) = 0;
        virtual WifiStatus::Values  getWifiStatus() = 0;
        virtual unsigned int        getCurrentTime() = 0;

    public:
        SmartHomeDevice(const WifiConfiguration&);
       ~SmartHomeDevice();

        void init() override;
        void go() override;
        void terminate() override;

        void onEvent(EventSystem*, const Event&) override;

        void sendHttpMessage(const HttpMessage_n::HttpMessage&);
    };
}