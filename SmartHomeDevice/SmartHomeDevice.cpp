#include "SmartHomeDevice.h"

namespace SmartHomeDevice_n
{
    #define FSM_CALLBACK_CLOSURE(func) [this](const EventData &eventData) { this->func(eventData); }

    SmartHomeDevice::SmartHomeDevice(const std::string &deviceName, const WifiConfiguration &configuration)
    : deviceName(deviceName),
      stateMachine(State::INITIAL),
      configuration(configuration),
      timerManager(nullptr),
      currentWifiStatus(WifiStatus::DISCONNECTED),
      currentServerConnStatus(false),
      debugDevice(nullptr)
    {
        debugDevice = new DebugDevice([this](const std::string &debugMessage)
        {
            this->debugPrint(debugMessage);
        });

        initParamsList();

        initEventSystem();

        initStateMachine();

        initTimers();

        initTaskManager();
    }

    SmartHomeDevice::~SmartHomeDevice()
    {
        if (timerManager != nullptr)
        {
            delete timerManager;
            timerManager = nullptr;
        }

        if (debugDevice != nullptr)
        {
            delete debugDevice;
            debugDevice = nullptr;
        }
    }

    void SmartHomeDevice::initParamsList()
    {
        // default mandatory params set

        auto deviceIdParam     = DeviceParameter("Device_ID",     DeviceParamType::TEXTBOX, true);
        auto deviceNameParam   = DeviceParameter("Device_Name",   DeviceParamType::TEXTBOX, true);
        auto deviceStatusParam = DeviceParameter("Device_Status", DeviceParamType::TEXTBOX, true);

        deviceIdParam.addValue("-1"); 
        deviceIdParam.setCurrentValue("-1");

        deviceNameParam.addValue(deviceName.empty() ? "SmartHomeDevice" : deviceName);
        deviceNameParam.setCurrentValue(deviceName.empty() ? "SmartHomeDevice" : deviceName);

        deviceStatusParam.addValue("Online");
        deviceStatusParam.setCurrentValue("Online");

        paramsList.push_back(deviceIdParam);
        paramsList.push_back(deviceNameParam);
        paramsList.push_back(deviceStatusParam);
    }

    void SmartHomeDevice::initEventSystem()
    {
        events[Events::START]                               = eventSystem.createEvent();
        events[Events::NETWORK_SCAN_RESULTS_READY]          = eventSystem.createEvent();
        events[Events::NETWORK_SCAN_TIMEOUT]                = eventSystem.createEvent();
        events[Events::NETWORK_SCAN_FAILED]                 = eventSystem.createEvent();
        events[Events::NETWORK_PICKED]                      = eventSystem.createEvent();
        events[Events::WIFI_CONNECTED]                      = eventSystem.createEvent();
        events[Events::WIFI_CONNECTION_FAILED]              = eventSystem.createEvent();
        events[Events::WIFI_CONNECTION_RETRIES_EXHAUSTED]   = eventSystem.createEvent();
        events[Events::WIFI_CONNECTION_TIMEOUT]             = eventSystem.createEvent();
        events[Events::SERVER_PICKED]                       = eventSystem.createEvent();
        events[Events::SERVER_CONNECTED]                    = eventSystem.createEvent();
        events[Events::SERVER_CONNECTION_FAILED]            = eventSystem.createEvent();
        events[Events::SERVER_CONNECTION_RETRIES_EXHAUSTED] = eventSystem.createEvent();
        events[Events::SERVER_CONNECTION_TIMEOUT]           = eventSystem.createEvent();
        events[Events::DEVICE_STATUS_REQUEST_TIMEOUT]       = eventSystem.createEvent();
        events[Events::DATA_AVAILABLE]                      = eventSystem.createEvent();
        events[Events::DISCONNECTED]                        = eventSystem.createEvent();
        events[Events::TIMER_EXPIRED]                       = eventSystem.createEvent();
        events[Events::FATAL_ERROR]                         = eventSystem.createEvent();

        // state machine subscriptions to events
        eventSystem.subscribe(events[Events::START],                         &stateMachine);
        eventSystem.subscribe(events[Events::NETWORK_SCAN_RESULTS_READY],    &stateMachine);
        eventSystem.subscribe(events[Events::NETWORK_SCAN_TIMEOUT],          &stateMachine);
        eventSystem.subscribe(events[Events::NETWORK_SCAN_FAILED],           &stateMachine);
        eventSystem.subscribe(events[Events::NETWORK_PICKED],                &stateMachine);
        eventSystem.subscribe(events[Events::WIFI_CONNECTED],                &stateMachine);
        eventSystem.subscribe(events[Events::WIFI_CONNECTION_FAILED],        &stateMachine);
        eventSystem.subscribe(events[Events::WIFI_CONNECTION_TIMEOUT],       &stateMachine);
        eventSystem.subscribe(events[Events::SERVER_PICKED],                 &stateMachine);
        eventSystem.subscribe(events[Events::SERVER_CONNECTED],              &stateMachine);
        eventSystem.subscribe(events[Events::SERVER_CONNECTION_FAILED],      &stateMachine);
        eventSystem.subscribe(events[Events::SERVER_CONNECTION_TIMEOUT],     &stateMachine);
        eventSystem.subscribe(events[Events::DEVICE_STATUS_REQUEST_TIMEOUT], &stateMachine);
        eventSystem.subscribe(events[Events::DATA_AVAILABLE],                &stateMachine);
        eventSystem.subscribe(events[Events::DISCONNECTED],                  &stateMachine);
        eventSystem.subscribe(events[Events::FATAL_ERROR],                   &stateMachine);

        // this class subscriptions to events
        eventSystem.subscribe(events[Events::TIMER_EXPIRED], this);
    }

    void SmartHomeDevice::initStateMachine()
    {
        stateMachine.setDebugDevice(debugDevice);

        stateMachine.addTransition(State::INITIAL,              events[Events::START],                               State::NETWORK_SCANNING,     FSM_CALLBACK_CLOSURE(fsm_startNetworksScan));
        stateMachine.addTransition(State::INITIAL,              events[Events::FATAL_ERROR],                         State::INITIAL,              FSM_CALLBACK_CLOSURE(fsm_handleFatalError));

        stateMachine.addTransition(State::NETWORK_SCANNING,     events[Events::NETWORK_SCAN_RESULTS_READY],          State::NETWORK_SCANNING,     FSM_CALLBACK_CLOSURE(fsm_tryToPickANetwork));
        stateMachine.addTransition(State::NETWORK_SCANNING,     events[Events::NETWORK_SCAN_FAILED],                 State::NETWORK_SCANNING,     FSM_CALLBACK_CLOSURE(fsm_startNetworksScan));
        stateMachine.addTransition(State::NETWORK_SCANNING,     events[Events::NETWORK_SCAN_TIMEOUT],                State::NETWORK_SCANNING,     FSM_CALLBACK_CLOSURE(fsm_startNetworksScan));
        stateMachine.addTransition(State::NETWORK_SCANNING,     events[Events::NETWORK_PICKED],                      State::CONNECTING_TO_WIFI,   FSM_CALLBACK_CLOSURE(fsm_connectToNetwork));
        stateMachine.addTransition(State::NETWORK_SCANNING,     events[Events::WIFI_CONNECTED],                      State::CONNECTING_TO_SERVER, FSM_CALLBACK_CLOSURE(fsm_startServerConnection));
        stateMachine.addTransition(State::NETWORK_SCANNING,     events[Events::FATAL_ERROR],                         State::INITIAL,              FSM_CALLBACK_CLOSURE(fsm_handleFatalError));
         
        stateMachine.addTransition(State::CONNECTING_TO_WIFI,   events[Events::WIFI_CONNECTED],                      State::CONNECTING_TO_SERVER, FSM_CALLBACK_CLOSURE(fsm_startServerConnection));
        stateMachine.addTransition(State::CONNECTING_TO_WIFI,   events[Events::NETWORK_SCAN_RESULTS_READY],          State::CONNECTING_TO_WIFI,   FSM_CALLBACK_CLOSURE(fsm_tryToPickANetwork));
        stateMachine.addTransition(State::CONNECTING_TO_WIFI,   events[Events::NETWORK_PICKED],                      State::CONNECTING_TO_WIFI,   FSM_CALLBACK_CLOSURE(fsm_connectToNetwork));
        stateMachine.addTransition(State::CONNECTING_TO_WIFI,   events[Events::WIFI_CONNECTION_FAILED],              State::CONNECTING_TO_WIFI,   FSM_CALLBACK_CLOSURE(fsm_connectToNetwork));
        stateMachine.addTransition(State::CONNECTING_TO_WIFI,   events[Events::WIFI_CONNECTION_TIMEOUT],             State::NETWORK_SCANNING,     FSM_CALLBACK_CLOSURE(fsm_startNetworksScan));
        stateMachine.addTransition(State::CONNECTING_TO_WIFI,   events[Events::WIFI_CONNECTION_RETRIES_EXHAUSTED],   State::NETWORK_SCANNING,     FSM_CALLBACK_CLOSURE(fsm_startNetworksScan));
        stateMachine.addTransition(State::CONNECTING_TO_WIFI,   events[Events::DISCONNECTED],                        State::NETWORK_SCANNING,     FSM_CALLBACK_CLOSURE(fsm_startNetworksScan));
        stateMachine.addTransition(State::CONNECTING_TO_WIFI,   events[Events::FATAL_ERROR],                         State::INITIAL,              FSM_CALLBACK_CLOSURE(fsm_handleFatalError));

        stateMachine.addTransition(State::CONNECTING_TO_SERVER, events[Events::SERVER_PICKED],                       State::CONNECTING_TO_SERVER, FSM_CALLBACK_CLOSURE(fsm_connectToServer));
        stateMachine.addTransition(State::CONNECTING_TO_SERVER, events[Events::SERVER_CONNECTED],                    State::CONNECTED,            FSM_CALLBACK_CLOSURE(fsm_handleConnectionToServer));
        stateMachine.addTransition(State::CONNECTING_TO_SERVER, events[Events::SERVER_CONNECTION_FAILED],            State::CONNECTING_TO_SERVER, FSM_CALLBACK_CLOSURE(fsm_connectToServer));
        stateMachine.addTransition(State::CONNECTING_TO_SERVER, events[Events::SERVER_CONNECTION_RETRIES_EXHAUSTED], State::NETWORK_SCANNING,     FSM_CALLBACK_CLOSURE(fsm_startNetworksScan));
        stateMachine.addTransition(State::CONNECTING_TO_SERVER, events[Events::SERVER_CONNECTION_TIMEOUT],           State::NETWORK_SCANNING,     FSM_CALLBACK_CLOSURE(fsm_startNetworksScan));
        stateMachine.addTransition(State::CONNECTING_TO_SERVER, events[Events::DISCONNECTED],                        State::NETWORK_SCANNING,     FSM_CALLBACK_CLOSURE(fsm_startNetworksScan));
        stateMachine.addTransition(State::CONNECTING_TO_SERVER, events[Events::FATAL_ERROR],                         State::INITIAL,              FSM_CALLBACK_CLOSURE(fsm_handleFatalError));

        stateMachine.addTransition(State::CONNECTED,            events[Events::DEVICE_STATUS_REQUEST_TIMEOUT],       State::CONNECTED,            FSM_CALLBACK_CLOSURE(fsm_requestDeviceStatus));
        stateMachine.addTransition(State::CONNECTED,            events[Events::DATA_AVAILABLE],                      State::CONNECTED,            FSM_CALLBACK_CLOSURE(fsm_readData));
        stateMachine.addTransition(State::CONNECTED,            events[Events::DISCONNECTED],                        State::NETWORK_SCANNING,     FSM_CALLBACK_CLOSURE(fsm_startNetworksScan));
        stateMachine.addTransition(State::CONNECTED,            events[Events::FATAL_ERROR],                         State::INITIAL,              FSM_CALLBACK_CLOSURE(fsm_handleFatalError));
    }

    void SmartHomeDevice::initTimers()
    {
        if (timerManager == nullptr)
        {
            timerManager = new TimerManager(&eventSystem, events[Events::TIMER_EXPIRED], [this]() -> unsigned int { return this->getCurrentTime(); });

            networkScanTimer         = timerManager->createTimer(configuration.networkScanTimeout);
            wifiConnectionTimer      = timerManager->createTimer(configuration.wifiConnectionTimeout);
            serverConnectionTimer    = timerManager->createTimer(configuration.serverConnectionTimeout);
            deviceStatusRequestTimer = timerManager->createTimer(configuration.deviceStatusRequestTimeout);
        }
    }

    void SmartHomeDevice::initTaskManager()
    {
        (void)taskManager.scheduleTask(this);
        (void)taskManager.scheduleTask(&eventSystem, Priority::HIGH);
        (void)taskManager.scheduleTask(timerManager, Priority::HIGH);
    }

    void SmartHomeDevice::init()
    {
        eventSystem.sendEvent(Event(events[Events::START]));
    }

    void SmartHomeDevice::go()
    {
        if (stateMachine.state() == State::CONNECTED)
        {
            // check connection to WiFi
            auto wifiStatus = getWifiStatus();
            if (wifiStatus != currentWifiStatus)
            {
                currentWifiStatus = wifiStatus;

                if (wifiStatus != WifiStatus::CONNECTED)
                {
                    eventSystem.sendEvent(Event(events[Events::DISCONNECTED]));

                    return;
                }
            }

            // check connection to the server
            auto serverConnectionStatus = connectedToServer();
            if (serverConnectionStatus != currentServerConnStatus)
            {
                currentServerConnStatus = serverConnectionStatus;

                if (!serverConnectionStatus)
                {
                    eventSystem.sendEvent(Event(events[Events::DISCONNECTED]));

                    return;
                }
            }

            // check if data is available to read
            if (dataAvailable())
                eventSystem.sendEvent(Event(events[Events::DATA_AVAILABLE]));
        }
    }

    void SmartHomeDevice::terminate()
    {
        disconnectFromServer();
        for (int i = 0; i < 10000; i++); // add some delay
        disconnectFromWiFi();
    }

    void SmartHomeDevice::onEvent(EventSystem *sender, const Event &event)
    {
        auto tmrToEventsMap = std::map<TimerHandle, EventId>
        {
            {networkScanTimer,         events[Events::NETWORK_SCAN_TIMEOUT]},
            {wifiConnectionTimer,      events[Events::WIFI_CONNECTION_TIMEOUT]},
            {serverConnectionTimer,    events[Events::SERVER_CONNECTION_TIMEOUT]},
            {deviceStatusRequestTimer, events[Events::DEVICE_STATUS_REQUEST_TIMEOUT]}
        };

        if (event.getId() == events[Events::TIMER_EXPIRED])
        {
            TimerHandle tmrId = INVALID_TIMER_HANDLE;

            event.getData(&tmrId, sizeof(tmrId));

            if (tmrToEventsMap.find(tmrId) != tmrToEventsMap.end())
                eventSystem.sendEvent(Event(tmrToEventsMap.at(tmrId)));
        }
    }

    void SmartHomeDevice::sendHttpMessage(const HttpMessage &msg)
    {
        if (msg.isValid() && connectedToServer())
            sendData(msg.rawText());
    }

    void SmartHomeDevice::addParam(const DeviceParameter &deviceParam)
    {
        if (std::find_if(paramsList.begin(), paramsList.end(), [&deviceParam](const auto &param) -> bool {return deviceParam.getName() == param.getName(); }) == paramsList.end())
        {
            paramsList.push_back(deviceParam);

            auto paramJson = deviceParam.toJson();

            // TODO: send "parameter added" notification to the server
        }
    }

    void SmartHomeDevice::setParamValue(const std::string &paramName, const std::string &paramValue)
    {
        auto param = std::find_if(paramsList.begin(), paramsList.end(), [&paramName](const auto &param) -> bool {return paramName == param.getName(); });

        if (param != paramsList.end())
        {
            param->setCurrentValue(paramValue);

            auto paramJson = param->toJson();

            // TODO: send "parameter value changed" notification to the server
        }
    }

    const std::string &SmartHomeDevice::getParamValue(const std::string &paramName)
    {
        auto param = std::find_if(paramsList.begin(), paramsList.end(), [&paramName](const auto &param) -> bool {return paramName == param.getName(); });

        if (param != paramsList.end())
            return param->getCurrentValue();
        else
            return std::string("");
    }

    // FSM callbacks

    void SmartHomeDevice::fsm_startNetworksScan(const EventData &eventData)
    {
        fsm_goIdle(eventData);

        timerManager->startTimer(networkScanTimer);

        scanForNetworks( [this, &eventData](int networksFound)
        {
            if (networksFound > 0)
            {
                auto netInfos = std::list<NetworkInfo>();

                for (int i = 0; i < networksFound; i++)
                    netInfos.push_back(getInfoForNetwork(i));

                netInfos.sort([](const NetworkInfo &first, const NetworkInfo &second) -> bool { return first.rssi > second.rssi; });

                for (auto netInfo : netInfos)
                {
                    EventData evData;
                    memset(&evData, 0, sizeof(evData));
                    evData.sender = eventData.sender;

                    memcpy(evData.data.networkInfo.ssid, netInfo.ssid, sizeof(MAX_SSID_LENGTH));
                    evData.data.networkInfo.isOpen  = netInfo.isOpen;
                    evData.data.networkInfo.rssi    = netInfo.rssi;
                    evData.data.networkInfo.channel = netInfo.channel;

                    this->eventSystem.sendEvent(Event(events[Events::NETWORK_SCAN_RESULTS_READY], &evData, sizeof(evData)));
                }
            }
            else
                this->eventSystem.sendEvent(Event(events[Events::NETWORK_SCAN_FAILED]));
        });
    }

    void SmartHomeDevice::fsm_tryToPickANetwork(const EventData &eventData)
    {
        if ( (configuration.knownNetworks.find(eventData.data.networkInfo.ssid) != configuration.knownNetworks.end()) || eventData.data.networkInfo.isOpen)
        {
            EventData evData = eventData;
            eventSystem.sendEvent(Event(events[Events::NETWORK_PICKED], &evData, sizeof(evData)));
        }
    }

    void SmartHomeDevice::fsm_connectToNetwork(const EventData &eventData)
    {
        if (getWifiStatus() == WifiStatus::CONNECTED)
        {
            wifiConnectionRetries = 0;

            timerManager->stopTimer(networkScanTimer);
            timerManager->stopTimer(wifiConnectionTimer);

            eventSystem.sendEvent(Event(events[Events::WIFI_CONNECTED]));
        }
        else
        {
            if (wifiConnectionRetries < configuration.maxWifiConnectionRetries)
            {
                timerManager->restartTimer(wifiConnectionTimer);

                std::string ssid = "";
                std::string pw   = "";

                if (!eventData.data.networkInfo.isOpen)
                {
                    auto networkCfg = configuration.knownNetworks.find(eventData.data.networkInfo.ssid);

                    if (networkCfg != configuration.knownNetworks.end())
                    {
                        ssid = networkCfg->first;
                        pw   = networkCfg->second;
                    }
                }
                else
                {
                    ssid = eventData.data.networkInfo.ssid;
                }

                connectToWiFi(ssid, pw);

                if (getWifiStatus() == WifiStatus::CONNECTED)
                {
                    wifiConnectionRetries = 0;

                    timerManager->stopTimer(networkScanTimer);
                    timerManager->stopTimer(wifiConnectionTimer);

                    eventSystem.sendEvent(Event(events[Events::WIFI_CONNECTED]));
                }
                else
                {
                    wifiConnectionRetries++;

                    EventData evData = eventData;
                    eventSystem.sendEvent(Event(events[Events::WIFI_CONNECTION_FAILED], &evData, sizeof(evData)));
                }
            }
            else
            {
                wifiConnectionRetries = 0;

                timerManager->stopAllTimers();

                eventSystem.sendEvent(Event(events[Events::WIFI_CONNECTION_RETRIES_EXHAUSTED]));
            }
        }
    }

    void SmartHomeDevice::fsm_startServerConnection(const EventData &eventData)
    {
        (void)eventData;

        if (getWifiStatus() == WifiStatus::CONNECTED)
        {
            if (connectedToServer())
                eventSystem.sendEvent(Event(events[Events::SERVER_CONNECTED]));
            else
            {

                if (!configuration.knownHosts.empty())
                {
                    fsm_goIdle(eventData);

                    timerManager->startTimer(serverConnectionTimer);

                    for (auto server : configuration.knownHosts)
                    {
                        EventData evData;
                        memset(&evData, 0, sizeof(evData));

                        evData.sender = eventData.sender;

                        memcpy(evData.data.hostInfo.host, server.first.c_str(), server.first.size());

                        evData.data.hostInfo.port = server.second;

                        eventSystem.sendEvent(Event(events[Events::SERVER_PICKED], &evData, sizeof(evData)));
                    }
                }
                else
                {
                    EventData evData;
                    memset(&evData, 0, sizeof(evData));

                    evData.sender = eventData.sender;

                    const char *errMsg = "Server connection is not possible: no known hosts in configuration!";
                    memcpy(evData.data.errorStr, errMsg, sizeof(errMsg));

                    eventSystem.sendEvent(Event(events[Events::FATAL_ERROR], &evData, sizeof(evData)));
                }
            }
        }
        else
            eventSystem.sendEvent(Event(events[Events::DISCONNECTED]));
    }

    void SmartHomeDevice::fsm_connectToServer(const EventData &eventData)
    {
        if (serverConnectionRetries < configuration.maxServerConnectionRetries)
        {
            connectToServer(eventData.data.hostInfo.host, eventData.data.hostInfo.port);

            if (connectedToServer())
            {
                connectedHost = eventData.data.hostInfo.host + ':' + std::to_string(eventData.data.hostInfo.port);
                eventSystem.sendEvent(Event(events[Events::SERVER_CONNECTED]));
            }
            else
            {
                serverConnectionRetries++;

                EventData evData = eventData;

                eventSystem.sendEvent(Event(events[Events::SERVER_CONNECTION_FAILED], &evData, sizeof(evData)));
            }
        }
        else
        {
            serverConnectionRetries = 0;

            timerManager->stopAllTimers();

            eventSystem.sendEvent(Event(events[Events::SERVER_CONNECTION_RETRIES_EXHAUSTED]));
        }
    }

    void SmartHomeDevice::fsm_handleFatalError(const EventData &eventData)
    {
        auto error = eventData.data.errorStr;
        
        *debugDevice << "Fatal error: " << error;

        // add some delay so the debug message can get to serial port
        for (int i = 0; i < 10000; i++);

        // stop all timers
        timerManager->stopAllTimers();

        // terminate all tasks
        taskManager.terminate();

        // reset the device
        reset();
    }

    void SmartHomeDevice::fsm_handleConnectionToServer(const EventData &eventData)
    {
        fsm_goIdle(eventData);

        auto deviceStatusJson = std::string(""); // TODO: fill up json fields using rapidjson

        HttpMessage deviceStatusMsg;
        
        deviceStatusMsg.setRequestLine(HttpMethod::PUT, "deviceStatus", HttpVersion::HTTP_1_1)
        .appendHeader(HttpHeader::HOST, connectedHost)
        .appendHeader(HttpHeader::ACCEPT, "application/json")
        .appendHeader(HttpHeader::CONTENT_TYPE, "application/json")
        .appendHeader(HttpHeader::CONTENT_LENGTH, std::to_string(deviceStatusJson.length()))
        .appendBody(deviceStatusJson);

        sendHttpMessage(deviceStatusMsg);

        timerManager->startTimer(deviceStatusRequestTimer);
    }

    void SmartHomeDevice::fsm_requestDeviceStatus(const EventData &eventData)
    {
        HttpMessage deviceRequestMsg;

        deviceRequestMsg.setRequestLine(HttpMethod::GET, "deviceStatus?id="+std::to_string(deviceId), HttpVersion::HTTP_1_1)
        .appendHeader(HttpHeader::HOST, connectedHost);

        sendHttpMessage(deviceRequestMsg);

        timerManager->restartTimer(deviceStatusRequestTimer);
    }

    void SmartHomeDevice::fsm_goIdle(const EventData &eventData)
    {
        timerManager->stopAllTimers();

        wifiConnectionRetries = 0;
        serverConnectionRetries = 0;
    }

    void SmartHomeDevice::fsm_readData(const EventData &eventData)
    {
        (void)eventData;

        if (dataAvailable())
        {
            HttpMessage httpMessage(readData());

            if (httpMessage.isValid())
            {
                // TODO: check received data here and send appropriate event...
            }
        }
    }
}