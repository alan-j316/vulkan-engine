#pragma once
/**
 * @file Logger.h
 * @author Alan Abraham P Kochumon
 * @date Created on: September 25, 2026
 *
 * @brief API abstraction for logger.
 *
 * @copyright Copyright (c) 2026 Alan Abraham P Kochumon
 */

#include "core/Api.h"

#include <format>
#include <string>

namespace tempest::log
{
    /// Initialize the logger. Must be called in the engine's init function.
    TEMPEST_API void init() noexcept;
    /// Shutdown the logger. Must be called in the engine's shutdown.
    TEMPEST_API void shutdown() noexcept;

    TEMPEST_API void info(std::string_view message) noexcept;
    TEMPEST_API void warn(std::string_view message) noexcept;
    TEMPEST_API void error(std::string_view message) noexcept;
} // namespace tempest::log