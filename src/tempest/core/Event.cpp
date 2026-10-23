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
#include "containers/PMRResource.h"
#include "containers/Vector.h"

#include <vector>

namespace tempest::core
{
    using EventCodeEntry = Vector<Listener*>;

    static constexpr auto MAX_EVENT_CODES = std::numeric_limits<EventCode>::max();

    struct EventSystemState
    {
        EventCodeEntry entries[MAX_EVENT_CODES];
    };

    static bool isInitialized = false;
    static EventSystemState state;
    static container::TempestPMR _allocator;


    bool event::init() noexcept
    {
        if (isInitialized)
        {
            return false;
        }

        for (auto& entry : state.entries)
        {
            entry = makeVector<Listener*>();
        }
        isInitialized = true;

        return true;
    }


    void event::shutdown() noexcept
    {
        for (auto& entry : state.entries)
        {
            entry.clear();
        }
    }


    bool registerEvent(const EventCode code, Listener* listener)
    {
        if (!isInitialized)
        {
            return false;
        }
        // TODO: Add checks for existing listeners

        state.entries[code].push_back(listener);
        return true;
    }


    bool unregisterEvent(const EventCode code, Listener* listener)
    {
        return std::erase(state.entries[code], listener) != 0;
    }


    bool fireEvent(const EventCode code, void* sender, const EventContext& context)
    {
        if (!isInitialized)
        {
            return false;
        }


        for (const auto& listener: state.entries[code])
        {
            if (listener->onEvent(code, sender, context))
            {
                // If event has been handled then there is no need to propogate it to
                // other listeners
                return true;
            }
        }

        // No listeners or none of the listeners handled the event
        return false;
    }
} // namespace tempest::core