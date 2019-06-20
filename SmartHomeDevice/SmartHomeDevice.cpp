#include "SmartHomeDevice.h"

namespace SmartHomeDevice_n
{
    #define FSM_CALLBACK_CLOSURE(func) [this](const EventData &eventData) { this->func(eventData); }

    SmartHomeDevice::SmartHomeDevice(const WifiConfiguration &configuration)
    : stateMachine(State::INITIAL),
      configuration(configuration),
      timerManager(nullptr)
    {
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
    }

    void SmartHomeDevice::initEventSystem()
    {
        events[Events::START]                      = eventSystem.createEvent();
        events[Events::NETWORK_SCAN_RESULTS_READY] = eventSystem.createEvent();
        events[Events::NETWORK_SCAN_TIMEOUT]       = eventSystem.createEvent();
        events[Events::NETWORK_SCAN_FAILED]        = eventSystem.createEvent();
        events[Events::NETWORK_PICKED]             = eventSystem.createEvent();
        events[Events::WIFI_CONNECTED]             = eventSystem.createEvent();
        events[Events::WIFI_CONNECTION_FAILED]     = eventSystem.createEvent();
        events[Events::WIFI_CONNECTION_TIMEOUT]    = eventSystem.createEvent();
        events[Events::SERVER_CONNECTED]           = eventSystem.createEvent();
        events[Events::SERVER_CONNECTION_FAILED]   = eventSystem.createEvent();
        events[Events::SERVER_CONNECTION_TIMEOUT]  = eventSystem.createEvent();
        events[Events::DISCONNECTED]               = eventSystem.createEvent();
        events[Events::TIMER_EXPIRED]              = eventSystem.createEvent();

        // state machine subscriptions to events
        eventSystem.subscribe(events[Events::START],                      &stateMachine);
        eventSystem.subscribe(events[Events::NETWORK_SCAN_RESULTS_READY], &stateMachine);
        eventSystem.subscribe(events[Events::NETWORK_SCAN_TIMEOUT],       &stateMachine);
        eventSystem.subscribe(events[Events::NETWORK_SCAN_FAILED],        &stateMachine);
        eventSystem.subscribe(events[Events::NETWORK_PICKED],             &stateMachine);
        eventSystem.subscribe(events[Events::WIFI_CONNECTED],             &stateMachine);
        eventSystem.subscribe(events[Events::WIFI_CONNECTION_FAILED],     &stateMachine);
        eventSystem.subscribe(events[Events::WIFI_CONNECTION_TIMEOUT],    &stateMachine);
        eventSystem.subscribe(events[Events::SERVER_CONNECTED],           &stateMachine);
        eventSystem.subscribe(events[Events::SERVER_CONNECTION_FAILED],   &stateMachine);
        eventSystem.subscribe(events[Events::SERVER_CONNECTION_TIMEOUT],  &stateMachine);
        eventSystem.subscribe(events[Events::DISCONNECTED],               &stateMachine);

        // this class subscriptions to events
        eventSystem.subscribe(events[Events::TIMER_EXPIRED], this);
    }

    void SmartHomeDevice::initStateMachine()
    {
        // TODO: change nullptr to the actual functions
        stateMachine.addTransition(State::INITIAL,              events[Events::START],                      State::NETWORK_SCANNING,     FSM_CALLBACK_CLOSURE(startNetworksScan));

        stateMachine.addTransition(State::NETWORK_SCANNING,     events[Events::NETWORK_SCAN_RESULTS_READY], State::NETWORK_SCANNING,     nullptr);
        stateMachine.addTransition(State::NETWORK_SCANNING,     events[Events::NETWORK_SCAN_FAILED],        State::NETWORK_SCANNING,     FSM_CALLBACK_CLOSURE(startNetworksScan));
        stateMachine.addTransition(State::NETWORK_SCANNING,     events[Events::NETWORK_SCAN_TIMEOUT],       State::NETWORK_SCANNING,     FSM_CALLBACK_CLOSURE(startNetworksScan));
        stateMachine.addTransition(State::NETWORK_SCANNING,     events[Events::NETWORK_PICKED],             State::CONNECTING_TO_WIFI,   nullptr);
         
        stateMachine.addTransition(State::CONNECTING_TO_WIFI,   events[Events::WIFI_CONNECTED],             State::CONNECTING_TO_SERVER, nullptr);
        stateMachine.addTransition(State::CONNECTING_TO_WIFI,   events[Events::WIFI_CONNECTION_FAILED],     State::CONNECTING_TO_WIFI,   nullptr);
        stateMachine.addTransition(State::CONNECTING_TO_WIFI,   events[Events::WIFI_CONNECTION_TIMEOUT],    State::CONNECTING_TO_WIFI,   nullptr);
        stateMachine.addTransition(State::CONNECTING_TO_WIFI,   events[Events::DISCONNECTED],               State::NETWORK_SCANNING,     nullptr);

        stateMachine.addTransition(State::CONNECTING_TO_SERVER, events[Events::SERVER_CONNECTED],           State::CONNECTED,            nullptr);
        stateMachine.addTransition(State::CONNECTING_TO_SERVER, events[Events::SERVER_CONNECTION_FAILED],   State::CONNECTING_TO_SERVER, nullptr);
        stateMachine.addTransition(State::CONNECTING_TO_SERVER, events[Events::SERVER_CONNECTION_TIMEOUT],  State::CONNECTING_TO_SERVER, nullptr);
        stateMachine.addTransition(State::CONNECTING_TO_SERVER, events[Events::DISCONNECTED],               State::NETWORK_SCANNING,     nullptr);

        stateMachine.addTransition(State::CONNECTED,            events[Events::DISCONNECTED],               State::NETWORK_SCANNING,     nullptr);
    }

    void SmartHomeDevice::initTimers()
    {
        if (timerManager == nullptr)
        {
            timerManager = new TimerManager(&eventSystem, events[Events::TIMER_EXPIRED], [this]() -> unsigned int { return this->getCurrentTime(); });

            networkScanTimer      = timerManager->createTimer(configuration.networkScanTimeout);
            wifiConnectionTimer   = timerManager->createTimer(configuration.wifiConnectionTimeout);
            serverConnectionTimer = timerManager->createTimer(configuration.serverConnectionTimeout);
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
        // TODO: not implemented
    }

    void SmartHomeDevice::terminate()
    {
        disconnectFromServer();
        for (int i = 0; i < 10000; i++); // add some delay
        disconnectFromWiFi();
    }

    void SmartHomeDevice::onEvent(EventSystem *sender, const Event &event)
    {
        static const auto tmrToEventsMap = std::map<TimerHandle, EventId>
        {
            {networkScanTimer,      events[Events::NETWORK_SCAN_TIMEOUT]},
            {wifiConnectionTimer,   events[Events::WIFI_CONNECTION_TIMEOUT]},
            {serverConnectionTimer, events[Events::SERVER_CONNECTION_TIMEOUT]}
        };

        if (event.getId() == events[Events::TIMER_EXPIRED])
        {
            TimerHandle tmrId = INVALID_TIMER_HANDLE;

            event.getData(&tmrId, sizeof(tmrId));

            if (tmrToEventsMap.find(tmrId) != tmrToEventsMap.end())
                eventSystem.sendEvent(Event(tmrToEventsMap.at(tmrId)));
        }
    }

    void SmartHomeDevice::sendHttpMessage(const HttpMessage_n::HttpMessage &msg)
    {
        if (msg.isValid() && connectedToServer())
            sendData(msg.rawText());
    }

    // FSM callbacks

    void SmartHomeDevice::startNetworksScan(const EventData &eventData)
    {
        if (timerManager->isTimerActive(networkScanTimer))
            timerManager->stopTimer(networkScanTimer);

        timerManager->startTimer(networkScanTimer);

        scanForNetworks( [this, &eventData](int networksFound)
        {
            if (networksFound > 0)
            {
                for (int i = 0; i < networksFound; i++)
                {      
                    EventData evData;
                    evData.sender = eventData.sender;
                    
                    auto netInfo = getInfoForNetwork(i);
                    
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
}