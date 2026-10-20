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
    // TODO: Rewrite using modern C++
    struct EventContext
    {
        uint64_t u64[2];
        int64_t i64[2];
        double f64[2];

        uint32_t u32[4];
        int32_t i32[4];
        float f32[4];

        uint16_t u16[8];
        int16_t i16[8];

        uint8_t u8[16];
        int8_t i8[16];
    };

    /// Event code type
    using EventCode = uint16_t;


    /// Internal system event codes. Application codes must start beyond 255.
    enum class SystemEventCode : uint8_t
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


    namespace event
    {
        bool init() noexcept;
        void shutdown() noexcept;
    } // namespace event

    class Listener
    {
    public:
        virtual ~Listener() = default;

        /// @brief Handler function for an event.
        /// @return `true` if event is handled.
        /// @returnf `false` otherwise.
        virtual bool onEvent(uint16_t code, void* sender, EventContext context) = 0;
    };

    /// @brief Register a event listener for the given @p code.
    /// @return `true` if successful.
    /// @return `false` otherwise.
    TEMPEST_API bool registerEvent(EventCode code, Listener& listener);

    /// @brief Unregister an event.
    TEMPEST_API bool unregisterEvent(EventCode code, Listener& listener);

    /// @brief Fire an event to all registered listeners for a given code.
    /// @param code The event code to fire.
    /// @param sender A pointer to the sender. Can be nullptr.
    /// @param context The event data.
    /// @return `true` if handled.
    /// @return `false` otherwise.
    TEMPEST_API bool fireEvent(EventCode code, void* sender, EventContext& context);


} // namespace tempest::core