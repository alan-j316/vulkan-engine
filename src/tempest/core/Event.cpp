/**
 * @file Event.cpp
 * @author Alan Abraham P Kochumon
 * @date Created on: October 17, 2026
 *
 * @brief Implementation of functions declared in Event.h
 *
 * @copyright Copyright (c) 2026 Alan Abraham P Kochumon
 */


#include "Event.h"

#include "MemorySubsystem.h"

#include <vector>

namespace tempest::core
{
    using EventCodeEntry = std::vector<Listener>;

    static constexpr auto MAX_EVENT_CODES = std::numeric_limits<EventCode>::max();

    struct EventSystemState
    {
        EventCodeEntry entries[MAX_EVENT_CODES];
    };

    static bool isInitialized = false;
    static EventSystemState state;


    bool registerEvent(EventCode code, Listener& listener) {}
    bool unregisterEvent(EventCode code, Listener& listener) {}
    bool fireEvent(EventCode code, void* sender, EventContext& context) {}
} // namespace tempest::core