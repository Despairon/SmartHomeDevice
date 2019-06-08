#include "EventSystem.h"

namespace EventSystem_n
{
    EventSystem &EventSystem::getInstance()
    {
        static EventSystem instance;

        return instance;
    }

    void EventSystem::subscribe(const EventID &event, EventSubscriber *subscriber)
    {
        if (subscriber != nullptr)
        {
            if (subscriptions.find(event) == subscriptions.end())
                subscriptions[event] = SubscribersList();

            if (std::find(subscriptions[event].begin(), subscriptions[event].end(), subscriber) == subscriptions[event].end())
                subscriptions[event].push_back(subscriber);
        }
    }

    void EventSystem::unsubscribe(const EventID &event, EventSubscriber *subscriber)
    {
        if (subscriber != nullptr)
        {
            if (subscriptions.find(event) != subscriptions.end())
                subscriptions[event].remove(subscriber);
        }
    }

    void EventSystem::sendEvent(const Event &event)
    {
        eventQueue.push(event);
    }

    void EventSystem::go()
    {
        if (!eventQueue.empty())
        {
            auto event = eventQueue.front();

            if (subscriptions.find(event.getId()) != subscriptions.end())
            {
                for (auto subscriber : subscriptions[event.getId()])
                    if (subscriber != nullptr)
                        subscriber->onEvent(event);
            }

            eventQueue.pop();
        }
    }
}