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

#include "Game.h"
#include "core/Event.h"
#include "core/Input.h"
#include "core/Logger.h"


namespace tempest::core
{
    static bool _isInitialized = false; // TODO: Update to TMP based singleton

    bool Application::create(Game* game) noexcept
    {
        if (_isInitialized)
        {
            log::error("Application already initialized");
            return false;
        }

        _game = game;

        // +=======================+
        // | Initialize Subsystems |
        // +=======================+
        log::init();

        if (!event::init())
        {
            log::error("Event system not initialized. Application cannot continue");
            return false;
        }
        // Input makes use of event system so that should be initialized first
        input::init();

        // Initialize app state
        _isRunning   = true;
        _isSuspended = false;

        const auto& [startPosX, startPosY, width, height, name] = _game->getConfig();
        if (!_platform.startup(name, startPosX, startPosY, width, height))
        {
            log::error("There was an error initializing the platform");
            return false;
        }

        _game->onResize(_width, _height);

        _isInitialized = true;

        return true;
    }


    bool Application::run() noexcept
    {
        while (_isRunning)
        {
            if (!_platform.pumpMessages())
                _isRunning = false;

            if (!_isSuspended)
            {
                // TODO: Update with delta time.
                if (!_game->update(0))
                {
                    log::error("Game update failed!");
                    _isRunning = false;
                    break;
                }

                if (!_game->render(0))
                {
                    log::error("Game render failed!");
                    _isRunning = false;
                    break;
                }

                // NOTE: State copy/update should be done before handling input
                input::update(0); // TODO: Update delta time.
            }
        }

        event::shutdown();
        input::shutdown();
        _platform.shutdown();

        return true;
    }
} // namespace tempest::core