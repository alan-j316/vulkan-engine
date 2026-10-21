/**
 * @file Input.cpp
 * @author Alan Abraham P Kochumon
 * @date Created on: October 21, 2026
 *
 * @brief Implementation of member functions declared in Input.h
 *
 * @copyright Copyright (c) 2026 Alan Abraham P Kochumon
 */

#include "Input.h"

#include "Event.h"
#include "Logger.h"

#include <limits>

namespace tempest::core
{
    static constexpr auto MAX_KEYCODES = std::numeric_limits<MouseButton_t>::max();

    struct KeyState
    {
        bool keys[MAX_KEYCODES];
    };

    struct MouseState
    {
        MousePosition position;
        uint8_t buttons[static_cast<size_t>(Button::MAX_BUTTONS)];
    };

    struct InputState
    {
        KeyState currentKeyState, previousKeyState;
        MouseState currentMouseState, previousMouseState;
    };


    static bool isInitialized = false;
    static InputState state{};



    void input::init() noexcept
    {
        isInitialized = true;
        log::info("Input system initialized!");
    }

    void input::shutdown() noexcept { isInitialized = false; }

    void input::update(double delta) noexcept
    {
        if (!isInitialized)
            return;

        state.previousKeyState   = state.currentKeyState;
        state.previousMouseState = state.currentMouseState;
    }

    void input::processKey(Keycode key, bool pressed)
    {
        const auto keyIdx = static_cast<size_t>(key);
        if (state.currentKeyState.keys[keyIdx] != pressed)
        {
            state.currentKeyState.keys[keyIdx] = pressed;

            EventContext context{};
            context.u16[0] = static_cast<uint16_t>(key);
            const auto eventCode =
                static_cast<EventCode>(pressed ? SystemEventCode::KEY_PRESSED : SystemEventCode::KEY_RELEASED);
            fireEvent(eventCode, nullptr, context);
        }
    }


    void input::processButton(Button button, bool pressed)
    {
        const auto btnIdx = static_cast<size_t>(button);
        // Only update the state if the state has changed
        if (state.currentMouseState.buttons[btnIdx] != pressed)
        {
            state.currentMouseState.buttons[btnIdx] = pressed;

            EventContext context{};
            context.u16[0]       = static_cast<uint16_t>(button);
            const auto eventCode = static_cast<EventCode>(pressed ? SystemEventCode::MOUSE_BUTTON_PRESSED
                                                                  : SystemEventCode::MOUSE_BUTTON_RELEASED);
            fireEvent(eventCode, nullptr, context);
        }
    }


    void input::processMouseMove(MousePosition position)
    {
        // Only update the state if position has changed
        if (state.currentMouseState.position != position)
        {
            state.currentMouseState.position = position;

            EventContext context{};
            context.u16[0] = position.x;
            context.u16[1] = position.y;

            fireEvent(static_cast<EventCode>(SystemEventCode::MOUSE_MOVED), nullptr, context);
        }
    }


    void input::processMouseScroll(int8_t zDelta)
    {
        EventContext context{};
        context.u8[0] = zDelta;
        fireEvent(static_cast<EventCode>(SystemEventCode::MOUSE_SCROLLED), nullptr, context);
    }


    bool input::isKeyDown(Keycode key)
    {
        if (!isInitialized)
            return false;
        return state.currentKeyState.keys[static_cast<MouseButton_t>(key)] == true;
    }

    bool input::isKeyUp(Keycode key)
    {
        if (!isInitialized)
            return true;
        return state.currentKeyState.keys[static_cast<MouseButton_t>(key)] == false;
    }

    bool input::wasKeyDown(Keycode key)
    {
        if (!isInitialized)
            return false;
        return state.previousKeyState.keys[static_cast<MouseButton_t>(key)] == true;
    }

    bool input::wasKeyUp(Keycode key)
    {
        if (!isInitialized)
            return true;
        return state.previousKeyState.keys[static_cast<MouseButton_t>(key)] == false;
    }

    bool input::isButtonDown(Button button)
    {
        if (!isInitialized)
            return false;
        return state.currentMouseState.buttons[static_cast<MouseButton_t>(button)] == true;
    }


    bool input::isButtonUp(Button button)
    {
        if (!isInitialized)
            return true;
        return state.currentMouseState.buttons[static_cast<MouseButton_t>(button)] == false;
    }


    bool input::wasButtonDown(Button button)
    {
        if (!isInitialized)
            return false;
        return state.previousMouseState.buttons[static_cast<MouseButton_t>(button)] == true;
    }


    bool input::wasButtonUp(Button button)
    {
        if (!isInitialized)
            return true;
        return state.previousMouseState.buttons[static_cast<MouseButton_t>(button)] == false;
    }


    MousePosition input::getMousePosition()
    {
        if (!isInitialized)
        {
            return MousePosition{};
        }
        return state.currentMouseState.position;
    }


    MousePosition input::getPreviousMousePosition()
    {
        if (!isInitialized)
        {
            return MousePosition{};
        }
        return state.previousMouseState.position;
    }

} // namespace tempest::core
