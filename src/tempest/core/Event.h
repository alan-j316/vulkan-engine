#pragma once
/**
 * @file Event.h
 * @author Alan Abraham P Kochumon
 * @date Created on: October 17, 2026
 *
 * @brief Event management system.
 *
 * @copyright Copyright (c) 2026 Alan Abraham P Kochumon
 */

#include "Api.h"

#include <bitset>
#include <cstdint>

namespace tempest::core
{

    struct EventContext
    {
        std::bitset<7> data;
        // TODO: Update
    };


    enum class EventCode : uint8_t
    {
        APPLICATION_QUIT      = 0x01,
        KEY_PRESSED           = 0x02,
        KEY_RELEASED          = 0x03,
        MOUSE_BUTTON_PRESSED  = 0x04,
        MOUSE_BUTTON_RELEASED = 0x05,
        MOUSE_MOVED           = 0x06,
        MOUSE_WHEEL_SCROLL    = 0x07,
        WINDOW_RESIZED        = 0x08,
        MAX_EVENT_CODE        = 0xFF
    };


    class Listener
    {
    public:
        virtual bool onEvent(EventCode code, void* sender, const EventContext& context) = 0;
    };


    /// Register to listen to events send to @p code.
    TEMPEST_API bool registerEvent(EventCode code, Listener* listener);

    /// Unregister the event listener from the event with the given @p code.
    TEMPEST_API bool unregisterEvent(EventCode code, Listener* listener);

    /// Fire an event to the listeners of the given code.
    TEMPEST_API bool fireEvent(EventCode code, void* sender, const EventContext& context);


} // namespace tempest::core