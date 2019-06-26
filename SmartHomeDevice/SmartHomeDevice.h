#pragma once

#include "HttpMessage.h"
#include "SmartHomeDeviceFsm.h"
#include "EventSystem.h"
#include "TaskManager.h"
#include "TimerManager.h"
#include "DebugDevice.h"

namespace SmartHomeDevice_n
{
    using namespace TaskManager_n;
    using namespace TimerManager_n;
    using namespace HttpMessage_n;
    using namespace DebugDevice_n;

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
        using KnownNetworks = std::map<std::string, std::string>;
        using KnownHosts    = std::map<std::string, unsigned short>;

        const KnownNetworks   knownNetworks;
        const KnownHosts      knownHosts;
        const unsigned short  networkScanTimeout;
        const unsigned short  wifiConnectionTimeout;
        const unsigned short  serverConnectionTimeout;
        const byte            maxWifiConnectionRetries;
        const byte            maxServerConnectionRetries;
    };

    class SmartHomeDevice : public EventSubscriber, public Task
    {
    private:
        EventSystem         eventSystem;
        SmartHomeDeviceFsm  stateMachine;
        TimerManager       *timerManager;
        TaskManager         taskManager;
        DebugDevice        *debugDevice;

        WifiConfiguration   configuration;

        std::map<Events::Values, EventId> events;

        // timers
        TimerHandle networkScanTimer;
        TimerHandle wifiConnectionTimer;
        TimerHandle serverConnectionTimer;

        // misc variables
        byte               wifiConnectionRetries;
        byte               serverConnectionRetries;
        WifiStatus::Values currentWifiStatus;
        bool               currentServerConnStatus;

        // init funcs
        void initEventSystem();
        void initStateMachine();
        void initTimers();
        void initTaskManager();

        // FSM callbacks
        void fsm_startNetworksScan(const EventData&);
        void fsm_tryToPickANetwork(const EventData&);
        void fsm_connectToNetwork(const EventData&);
        void fsm_startServerConnection(const EventData&);
        void fsm_connectToServer(const EventData&);
        void fsm_handleFatalError(const EventData&);
        void fsm_goIdle(const EventData&);
        void fsm_readData(const EventData&);

    protected:
        // WiFi interface
        virtual void                connectToWiFi(const std::string &ssid, const std::string &password) = 0;
        virtual void                disconnectFromWiFi() = 0;
        virtual void                scanForNetworks(std::function<void(int)> scanCallback) = 0;
        virtual NetworkInfo         getInfoForNetwork(const byte &networkNumber) = 0;
        virtual void                connectToServer(const std::string &host, const unsigned short &port) = 0;
        virtual void                disconnectFromServer() = 0;
        virtual bool                dataAvailable() = 0;
        virtual bool                connectedToServer() = 0;
        virtual std::string         readData() = 0;
        virtual void                sendData(const std::string &textData) = 0;
        virtual WifiStatus::Values  getWifiStatus() = 0;
        virtual unsigned int        getCurrentTime() = 0;
        virtual void                reset() = 0;
        virtual void                debugPrint(const std::string &debugMessage) = 0;

        void sendHttpMessage(const HttpMessage&);
    public:
        SmartHomeDevice(const WifiConfiguration&);
       ~SmartHomeDevice();

        void init() override;
        void go() override;
        void terminate() override;

        void onEvent(EventSystem*, const Event&) override;
    };
}