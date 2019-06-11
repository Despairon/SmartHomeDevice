#pragma once

#include "HttpMessage.h"
#include "SmartHomeDeviceFsm.h"
#include "EventSystem.h"
#include "TaskManager.h"

namespace SmartHomeDevice_n
{
    using namespace TaskManager_n;

    class SmartHomeDevice : public EventSubscriber, public Task
    {
    private:
        EventSystem        eventSystem;
        TaskManager        taskManager;
        SmartHomeDeviceFsm stateMachine;

        void initEventSystem();
        void initStateMachine();
        void initTaskManager();
    public:
        SmartHomeDevice();

        void init() override;
        void go() override;
        void terminate() override;

        void onEvent(EventSystem*, const Event&) override;
    };
}