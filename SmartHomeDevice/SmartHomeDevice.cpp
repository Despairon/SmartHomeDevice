#include "SmartHomeDevice.h"

namespace SmartHomeDevice_n
{
    SmartHomeDevice::SmartHomeDevice(const WifiConfiguration &configuration)
    : stateMachine(State::INITIAL),
      configuration(configuration)
    {
        initEventSystem();

        initStateMachine();

        initTaskManager();
    }

    void SmartHomeDevice::initEventSystem()
    {
        events[Events::START]                      = eventSystem.createEvent();
        events[Events::NETWORK_SCAN_RESULTS_READY] = eventSystem.createEvent();
        events[Events::NETWORK_SCAN_TIMEOUT]       = eventSystem.createEvent();
        events[Events::WIFI_CONNECTED]             = eventSystem.createEvent();
        events[Events::WIFI_CONNECTION_FAILED]     = eventSystem.createEvent();
        events[Events::WIFI_CONNECTION_TIMEOUT]    = eventSystem.createEvent();
        events[Events::SERVER_CONNECTED]           = eventSystem.createEvent();
        events[Events::SERVER_CONNECTION_FAILED]   = eventSystem.createEvent();
        events[Events::SERVER_CONNECTION_TIMEOUT]  = eventSystem.createEvent();
        events[Events::DISCONNECTED]               = eventSystem.createEvent();

        eventSystem.subscribe(events[Events::START],                      &stateMachine);
        eventSystem.subscribe(events[Events::NETWORK_SCAN_RESULTS_READY], &stateMachine);
        eventSystem.subscribe(events[Events::NETWORK_SCAN_TIMEOUT],       &stateMachine);
        eventSystem.subscribe(events[Events::WIFI_CONNECTED],             &stateMachine);
        eventSystem.subscribe(events[Events::WIFI_CONNECTION_FAILED],     &stateMachine);
        eventSystem.subscribe(events[Events::WIFI_CONNECTION_TIMEOUT],    &stateMachine);
        eventSystem.subscribe(events[Events::SERVER_CONNECTED],           &stateMachine);
        eventSystem.subscribe(events[Events::SERVER_CONNECTION_FAILED],   &stateMachine);
        eventSystem.subscribe(events[Events::SERVER_CONNECTION_TIMEOUT],  &stateMachine);
        eventSystem.subscribe(events[Events::DISCONNECTED],               &stateMachine);
    }

    void SmartHomeDevice::initStateMachine()
    {
        // TODO: change nullptr to the actual functions
        stateMachine.addTransition(State::INITIAL,              events[Events::START],                      State::NETWORK_SCANNING,     nullptr);

        stateMachine.addTransition(State::NETWORK_SCANNING,     events[Events::NETWORK_SCAN_RESULTS_READY], State::CONNECTING_TO_WIFI,   nullptr);
        stateMachine.addTransition(State::NETWORK_SCANNING,     events[Events::NETWORK_SCAN_TIMEOUT],       State::NETWORK_SCANNING,     nullptr);
         
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

    void SmartHomeDevice::initTaskManager()
    {
        taskManager.scheduleTask(this);
        taskManager.scheduleTask(&eventSystem, Priority::HIGH);
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
        // TODO: not implemented
    }

    void SmartHomeDevice::sendHttpMessage(const HttpMessage_n::HttpMessage &msg)
    {
        if (msg.isValid() && connectedToServer())
            sendData(msg.rawText());
    }
}