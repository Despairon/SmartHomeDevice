#include "SmartHomeDeviceFsm.h"

namespace SmartHomeDevice_n
{
    SmartHomeDeviceFsm::SmartHomeDeviceFsm(const State &initialState) : StateMachine(initialState) { }

    std::string SmartHomeDeviceFsm::stateToString(const State &state) const
    {
        switch (state)
        {
            default: return "UNKNOWN_STATE";
        }
    }

    std::string SmartHomeDeviceFsm::eventToString(const EventId &event) const
    {
        switch (event)
        {
            default: return "UNKNOWN_EVENT";
        }
    }

    void SmartHomeDeviceFsm::onEvent(EventSystem* sender, const Event &event)
    {
        EventData eventData;

        event.getData(&eventData.data, sizeof(eventData.data));

        execute(event.getId(), eventData, &std::cout);
    }
}