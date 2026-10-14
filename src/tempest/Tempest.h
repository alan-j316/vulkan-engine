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

namespace tempest
{
    class TempestEngine
    {
    public:
        TempestEngine() noexcept = default;

        /// Start the engine and its subsystems.
        bool startup() noexcept;

        /// Start the main engine loop.
        void run() noexcept;

        /// Shutdown the engine and cleanup the resources.
        void shutdown() noexcept;
    };
} // namespace tempest