/**
 * @file EventListeners.cpp
 * @author Alan Abraham P Kochumon
 * @date Created on: October 21, 2026
 *
 * @brief Implementation of Event Listeners.
 *
 * @copyright Copyright (c) 2026 Alan Abraham P Kochumon
 */

#include "EventListeners.h"

#include "Application.h"
#include "Input.h"
#include "core/Logger.h"

namespace tempest::core
{

    bool ApplicationEventListener::onEvent(EventCode code, void* sender, EventContext context)
    {
        switch (static_cast<SystemEventCode>(code))
        {
            case SystemEventCode::APPLICATION_QUIT:
                _app._isRunning = false;
                return true;
            default:
                return false;
        }
    }


    bool KeyEventListener::onEvent(EventCode code, void* sender, EventContext context)
    {
        if (static_cast<SystemEventCode>(code) == SystemEventCode::KEY_PRESSED)
        {
            const auto keyCode = static_cast<Keycode>(context.u16[0]);
            if (keyCode == Keycode::ESCAPE)
            {
                fireEvent(static_cast<EventCode>(SystemEventCode::APPLICATION_QUIT), nullptr, {});
                return false;
            }
            else if (keyCode == Keycode::A)
            {
                log::info("A key was pressed");
            }
            else
            {
                log::info("Key was pressed");
            }
        }
        else if (static_cast<SystemEventCode>(code) == SystemEventCode::KEY_RELEASED)
        {
            const auto keyCode = static_cast<Keycode>(context.u16[0]);
            if (keyCode == Keycode::C)
            {
                log::info("C was released");
            }
            else
            {
                log::info("Another key was released");
            }
        }
        return false;
    }
} // namespace tempest::core