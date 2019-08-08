#pragma once

#include "HttpMessage.h"
#include "SmartHomeDeviceFsm.h"
#include "EventSystem.h"
#include "TaskManager.h"
#include "TimerManager.h"
#include "DebugDevice.h"
#include "DeviceParameter.h"

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
        const unsigned short  deviceStatusRequestTimeout;
        const byte            maxWifiConnectionRetries;
        const byte            maxServerConnectionRetries;
    };

    class SmartHomeDevice : public EventSubscriber, public Task
    {
    private:
        unsigned long deviceId;
        std::string   deviceName;

        EventSystem         eventSystem;
        SmartHomeDeviceFsm  stateMachine;
        TimerManager       *timerManager;
        TaskManager         taskManager;
        DebugDevice        *debugDevice;

        WifiConfiguration   configuration;

        std::map<Events::Values, EventId> events;

        std::list<DeviceParameter> paramsList;

        // timers
        TimerHandle networkScanTimer;
        TimerHandle wifiConnectionTimer;
        TimerHandle serverConnectionTimer;
        TimerHandle deviceStatusRequestTimer;

        // misc variables
        std::string          connectedHost;
        byte                 wifiConnectionRetries;
        byte                 serverConnectionRetries;
        WifiStatus::Values   currentWifiStatus;
        bool                 currentServerConnStatus;

        // init funcs
        void initParamsList();
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
        void fsm_handleConnectionToServer(const EventData&);
        void fsm_requestDeviceStatus(const EventData&);
        void fsm_goIdle(const EventData&);
        void fsm_readData(const EventData&);
        void fsm_saveDeviceId(const EventData&);
        void fsm_handleDeviceIdError(const EventData&);

        SmartHomeDevice() = delete;

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

        bool addParam(const DeviceParameter&);
        bool setParamValue(const std::string&, const std::string&);
        const std::string &getParamValue(const std::string&);
    public:
        SmartHomeDevice(const std::string&, const WifiConfiguration&);
       ~SmartHomeDevice();

        void init() override;
        void go() override;
        void terminate() override;

		void run(); // use this method in a loop for the system to be in working state.

        void onEvent(EventSystem*, const Event&) override;
    };
}