/**
 * @file Application.cpp
 * @author Alan Abraham P Kochumon
 * @date Created on: October 17, 2026
 *
 * @brief Implementation of member functions declared in Application.h
 *
 * @copyright Copyright (c) 2026 Alan Abraham P Kochumon
 */

#include "Application.h"

#include "core/Logger.h"


namespace tempest::core
{
    static bool _isInitialized = false; // TODO: Update to TMP based singleton

    bool Application::create(const AppConfig& config) noexcept
    {
        if (_isInitialized)
        {
            log::error("Application already initialized");
            return false;
        }

        // +=======================+
        // | Initialize Subsystems |
        // +=======================+
        log::init();

        // Initialize app state
        _isRunning   = true;
        _isSuspended = false;

        if (!_platform.startup(config.name, config.startPosX, config.startPosY, config.width, config.height))
        {
            log::error("There was an error initializing the platform");
            return false;
        }
        _isInitialized = true;

        return true;
    }


    bool Application::run() noexcept
    {
        while (true)
        {
            if (!_platform.pumpMessages())
                _isRunning = false;
        }
        _platform.shutdown();

        return true;
    }
} // namespace tempest::core