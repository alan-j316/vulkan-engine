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

    void init() noexcept
    {
        if (_coreLogger != nullptr)
            return;

        quill::Backend::start();
        _coreLogger = quill::simple_logger();
    }

    void shutdown() noexcept { _coreLogger = nullptr; }

    void info(const std::string_view message) noexcept { quill::info(_coreLogger, "{}\n", message); }

    void warn(const std::string_view message) noexcept { quill::warning(_coreLogger, "{}\n", message); }

    void error(const std::string_view message) noexcept { quill::error(_coreLogger, "{}\n", message); }

} // namespace tempest::log