#pragma once
/**
 * @file Application.h
 * @author Alan Abraham P Kochumon
 * @date Created on: October 17, 2026
 *
 * @brief Application
 *
 * @copyright Copyright (c) 2026 Alan Abraham P Kochumon
 */

#include "Api.h"
#include "AppConfig.h"
#include "platform/Platform.h"

namespace tempest::core
{

    class Application
    {
    public:
        TEMPEST_API bool create(const AppConfig& config) noexcept;
        TEMPEST_API bool run() noexcept;

    private:
        bool _isRunning{};
        bool _isSuspended{};
        platform::Platform _platform{};
        uint32_t _width{}, _height{};
        double _lastTime{};
    };
} // namespace tempest::core