#pragma once
/**
 * @file Tempest.h
 * @author Alan Abraham P Kochumon
 * @date Created on: October 14, 2026
 *
 * @brief Tempest Engine.
 *
 * @copyright Copyright (c) 2026 Alan Abraham P Kochumon
 */

#include "core/Api.h"
#include "core/Application.h"

namespace tempest
{
    class TempestEngine
    {
    public:
        TempestEngine() noexcept = default;

        /// Start the engine and its subsystems.
        TEMPEST_API bool startup() noexcept;

        /// Start the main engine loop.
        TEMPEST_API void run() noexcept;

        /// Shutdown the engine and cleanup the resources.
        TEMPEST_API void shutdown() noexcept;
    };
} // namespace tempest