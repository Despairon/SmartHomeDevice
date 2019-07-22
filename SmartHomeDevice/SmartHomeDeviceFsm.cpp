#include "SmartHomeDeviceFsm.h"

namespace SmartHomeDevice_n
{
    SmartHomeDeviceFsm::SmartHomeDeviceFsm(const State::Values &initialState) : StateMachine(initialState), debugDevice(nullptr) { }

    std::string SmartHomeDeviceFsm::stateToString(const State::Values &state) const
    {
        switch (state)
        {
            case State::INITIAL:              return "INITIAL";
            case State::NETWORK_SCANNING:     return "NETWORK_SCANNING";
            case State::CONNECTING_TO_WIFI:   return "CONNECTING_TO_WIFI";
            case State::CONNECTING_TO_SERVER: return "CONNECTING_TO_SERVER";
            case State::CONNECTED:            return "CONNECTED";
            
        default: return "UNKNOWN_STATE";
        }
    }

    std::string SmartHomeDeviceFsm::eventToString(const EventId &event) const
    {
        switch (event)
        {
            case Events::START:                                 return "START";
            case Events::NETWORK_SCAN_RESULTS_READY:            return "NETWORK_SCAN_RESULTS_READY";
            case Events::NETWORK_SCAN_FAILED:                   return "NETWORK_SCAN_FAILED";
            case Events::NETWORK_SCAN_TIMEOUT:                  return "NETWORK_SCAN_TIMEOUT";
            case Events::NETWORK_PICKED:                        return "NETWORK_PICKED";
            case Events::WIFI_CONNECTED:                        return "WIFI_CONNECTED";
            case Events::WIFI_CONNECTION_FAILED:                return "WIFI_CONNECTION_FAILED";
            case Events::WIFI_CONNECTION_RETRIES_EXHAUSTED:     return "WIFI_CONNECTION_RETRIES_EXHAUSTED";
            case Events::WIFI_CONNECTION_TIMEOUT:               return "WIFI_CONNECTION_TIMEOUT";
            case Events::SERVER_PICKED:                         return "SERVER_PICKED";
            case Events::SERVER_CONNECTED:                      return "SERVER_CONNECTED";
            case Events::SERVER_CONNECTION_FAILED:              return "SERVER_CONNECTION_FAILED";
            case Events::SERVER_CONNECTION_RETRIES_EXHAUSTED:   return "SERVER_CONNECTION_RETRIES_EXHAUSTED";
            case Events::SERVER_CONNECTION_TIMEOUT:             return "SERVER_CONNECTION_TIMEOUT";
            case Events::DEVICE_STATUS_REQUEST_TIMEOUT:         return "DEVICE_STATUS_REQUEST_TIMEOUT";
            case Events::DATA_AVAILABLE:                        return "DATA_AVAILABLE";
            case Events::DEVICE_ID_RECEIVED:                    return "DEVICE_ID_RECEIVED";
            case Events::DEVICE_ID_ERROR:                       return "DEVICE_ID_ERROR";
            case Events::DISCONNECTED:                          return "DISCONNECTED";
            case Events::TIMER_EXPIRED:                         return "TIMER_EXPIRED";
            case Events::FATAL_ERROR:                           return "FATAL_ERROR";
            
            default: return "UNKNOWN_EVENT";
        }
    }

    void SmartHomeDeviceFsm::onEvent(EventSystem* sender, const Event &event)
    {
        EventData eventData;

        eventData.sender = sender;

        event.getData(&eventData.data, sizeof(eventData.data));

        execute(event.getId(), eventData, debugDevice);
    }

    void SmartHomeDeviceFsm::setDebugDevice(DebugDevice *debugDevice)
    {
        this->debugDevice = debugDevice;
    }
}