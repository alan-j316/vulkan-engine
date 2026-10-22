#pragma once
/**
 * @file Platform.h
 * @author Alan Abraham P Kochumon
 * @date Created on: October 16, 2026
 *
 * @brief Platform API layer.
 *
 * @copyright Copyright (c) 2026 Alan Abraham P Kochumon
 */

#include "Memory.h"
#include "PlatformDetect.h"

#include <string>


namespace tempest::platform
{
    /// Forward declares the internal platform state.
    struct PlatformState;


    class Platform
    {
    public:
        bool startup(std::string_view appName, uint32_t x, uint32_t y, uint32_t width, uint32_t height);
        void shutdown() const;
        bool pumpMessages();

        constexpr PlatformState* getState() const noexcept { return _state; };

    private:
        PlatformState* _state{};
    };


} // namespace tempest::platform