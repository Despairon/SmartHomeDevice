#include "SmartHomeDeviceFsm.h"

namespace SmartHomeDevice_n
{
    SmartHomeDeviceFsm::SmartHomeDeviceFsm(const State::Values &initialState) : StateMachine(initialState) { }

    std::string SmartHomeDeviceFsm::stateToString(const State::Values &state) const
    {
        switch (state)
        {
            case State::Values::INITIAL:              return "INITIAL";
            case State::Values::NETWORK_SCANNING:     return "NETWORK_SCANNING";
            case State::Values::CONNECTING_TO_WIFI:   return "CONNECTING_TO_WIFI";
            case State::Values::CONNECTING_TO_SERVER: return "CONNECTING_TO_SERVER";
            case State::Values::CONNECTED:            return "CONNECTED";
            
        default: return "UNKNOWN_STATE";
        }
    }

    std::string SmartHomeDeviceFsm::eventToString(const EventId &event) const
    {
        switch (event)
        {
            case Events::Values::START:                      return "START";
            case Events::Values::NETWORK_SCAN_RESULTS_READY: return "NETWORK_SCAN_RESULTS_READY";
            case Events::Values::NETWORK_SCAN_FAILED:        return "NETWORK_SCAN_FAILED";
            case Events::Values::NETWORK_SCAN_TIMEOUT:       return "NETWORK_SCAN_TIMEOUT";
            case Events::Values::NETWORK_PICKED:             return "NETWORK_PICKED";
            case Events::Values::WIFI_CONNECTED:             return "WIFI_CONNECTED";
            case Events::Values::WIFI_CONNECTION_FAILED:     return "WIFI_CONNECTION_FAILED";
            case Events::Values::WIFI_CONNECTION_TIMEOUT:    return "WIFI_CONNECTION_TIMEOUT";
            case Events::Values::SERVER_CONNECTED:           return "SERVER_CONNECTED";
            case Events::Values::SERVER_CONNECTION_FAILED:   return "SERVER_CONNECTION_FAILED";
            case Events::Values::SERVER_CONNECTION_TIMEOUT:  return "SERVER_CONNECTION_TIMEOUT";
            case Events::Values::DISCONNECTED:               return "DISCONNECTED";
            
            default: return "UNKNOWN_EVENT";
        }
    }

    void SmartHomeDeviceFsm::onEvent(EventSystem* sender, const Event &event)
    {
        EventData eventData;

        eventData.sender = sender;

        event.getData(&eventData.data, sizeof(eventData.data));

        execute(event.getId(), eventData, &std::cout);
    }
}