#pragma once

/*
* This file contains State Machine template declarations and definitions
*/
#include <string>
#include <map>
#include <functional>

namespace FSM
{
#define FSM_DEBUG(device) if(device != nullptr) (*device)

    template <typename StateType, typename EventType, typename EventDataType, typename DebugIODeviceType>
    class StateMachine
    {
    public:
        using StateMachineCallback = std::function<void(const EventDataType&)>;                                                            // state machine callback name association
        using TransitionsMap       = std::map<StateType, std::map<EventType, std::pair<StateType, StateMachineCallback>>>;                 // transitions map name association
            
        struct Transition                                                                                                                  // transition structure 
        {
            StateType            currentState;
            EventType            event;
            StateType            nextState;
            StateMachineCallback action;
        };
    private:
        StateType      _state;                                                                                                              // current state
        TransitionsMap transitions;                                                                                                         // transitions map
    public:
        StateMachine(const StateType &initialState)                                                                                         // constructor
        {
            _state = initialState;

            transitions.clear();
        }

        ~StateMachine() { }                                                                                                                 // destructor

        void addTransition(const StateType &state, const EventType &event, const StateType &nextState, StateMachineCallback action)         // add transition method
        {
            transitions[state][event] = make_pair(nextState, action);
        }

        void execute(const EventType &event, const EventDataType &eventData, DebugIODeviceType *debugDevice = nullptr)                      // execute state machine method
        {
            if (!transitions.empty())
            {
                try
                {
                    if (transitions.at(_state).at(event).second != nullptr)
                        transitions.at(_state).at(event).second(eventData);

                    FSM_DEBUG(debugDevice) << "Transition from " << stateToString(_state).c_str()
                        << " to " << stateToString(transitions.at(_state).at(event).first).c_str()
                        << " by event " << eventToString(event).c_str()
                        << " successful!\n";

                    _state = transitions.at(_state).at(event).first;
                }
                catch (std::exception &e)
                {
                    FSM_DEBUG(debugDevice) << "Transition from " << stateToString(_state).c_str()
                        << " by event " << eventToString(event).c_str()
                        << " not found!\n";
                }
            }
            else
            {
                FSM_DEBUG(debugDevice) << "Transitions table is empty!";
            }
        }

        std::string virtual stateToString(const StateType&) const = 0;                                                                         // abstract state to string method
        std::string virtual eventToString(const EventType&) const = 0;                                                                         // abstract event to string method

        const StateType &state() const                                                                                                         // 'state' getter
        {
            return _state;
        }
    };

}