/**
 * @file Logger.cpp
 * @author Alan Abraham P Kochumon
 * @date Created on: September 25, 2026
 *
 * @brief Implementation of Logging function declared in Logger.h
 *
 * @copyright Copyright (c) 2026 Alan Abraham P Kochumon
 */


#include "Logger.h"

#include <memory>
#include <quill/LogFunctions.h>
#include <quill/SimpleSetup.h>

namespace tempest::log
{

    static quill::Logger* _coreLogger;

    TEMPEST_API void init() noexcept
    {
        if (_coreLogger != nullptr)
            return;

        quill::Backend::start();
        _coreLogger = quill::simple_logger();
    }

    TEMPEST_API void shutdown() noexcept { _coreLogger = nullptr; }

    TEMPEST_API void info(const std::string_view message) noexcept { quill::info(_coreLogger, "{}\n", message); }

    TEMPEST_API void warn(const std::string_view message) noexcept { quill::warning(_coreLogger, "{}\n", message); }

    TEMPEST_API void error(const std::string_view message) noexcept { quill::error(_coreLogger, "{}\n", message); }

} // namespace tempest::log