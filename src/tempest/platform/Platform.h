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

namespace tempest::platform
{
    /// Defines the internal platform state.
    /// Must be defined in each platform specialization compilation units.
    template <PlatformType Platform>
    struct PlatformState
    {};


    class Platform
    {
    public:
        void shutdown();
        void start();
        void pumpMessages();

    private:
        PlatformState<CurrentPlatform> _state;
    };


} // namespace tempest::platform