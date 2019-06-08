#pragma once

#include <queue>
#include <map>
#include <list>

#include "EventSubscriber.h"

namespace EventSystem_n
{
    class EventSystem
    {
        using SubscribersList  = std::list<EventSubscriber*>;
        using Subscriptions    = std::map<EventID, SubscribersList>;
        using EventQueue       = std::queue<Event>;

    private:
        Subscriptions subscriptions;
        EventQueue    eventQueue;

        EventSystem() = default;
        EventSystem operator=(const EventSystem&) = delete;
        EventSystem(const EventSystem&) = delete;
    public:
        static EventSystem &getInstance();

        void subscribe(const EventID&, EventSubscriber*);
        void unsubscribe(const EventID&, EventSubscriber*);
        void sendEvent(const Event&);

        void go();
    };
}