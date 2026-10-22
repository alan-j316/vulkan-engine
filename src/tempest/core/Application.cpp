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
#include "MemorySubsystem.h"
#include "core/Event.h"
#include "core/EventListeners.h"
#include "core/Input.h"
#include "core/Logger.h"
#include "platform/ClockTime.h"
#include "renderer/RendererFrontend.h"

#include <format>


namespace tempest::core
{
    static bool _isInitialized = false; // TODO: Update to TMP based singleton

    Application::Application() noexcept: _appEventListener(*this), _keyEventListener(*this) {}


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

        // TODO: Make renderer selectable
        if (!_renderer->init(renderer::RendererType::VULKAN, game->getConfig().name, _platform.getState()))
        {
            log::error("There was an error initializing the renderer");
            return false;
        }

        if (!_game->initialize())
        {
            log::error("There was an error initializing the game");
            return false;
        }
        _game->onResize(_width, _height);

        // Event Listener
        registerEvent(static_cast<EventCode>(SystemEventCode::APPLICATION_QUIT), &_appEventListener);
        registerEvent(static_cast<EventCode>(SystemEventCode::KEY_PRESSED), &_keyEventListener);
        registerEvent(static_cast<EventCode>(SystemEventCode::KEY_RELEASED), &_keyEventListener);

        _isInitialized = true;

        return true;
    }


    bool Application::run() noexcept
    {
        log::info(memory::getMemUsageStr());
        _clock.start();
        _clock.update();
        _lastTime = _clock.elapsedTime;

        double runningTime{};
        uint64_t frameCount{};

        while (_isRunning)
        {
            _platform.pumpMessages();
            // if (!_platform.pumpMessages())
            //     _isRunning = false;

            if (!_isSuspended)
            {
                // Update clock time
                _clock.update();
                const double currentTime    = _clock.elapsedTime;
                const double deltaTime      = currentTime - _lastTime;
                const double frameStartTime = platform::getAbsoluteTime();

                if (!_game->update(static_cast<float>(deltaTime)))
                {
                    log::error("Game update failed!");
                    _isRunning = false;
                    break;
                }

                if (!_game->render(static_cast<float>(deltaTime)))
                {
                    log::error("Game render failed!");
                    _isRunning = false;
                    break;
                }
                renderer::RenderPacket packet{ .deltaTime = static_cast<float>(deltaTime) };
                _renderer->drawFrame(packet);

                // Frame end time
                const double frameEndTime = platform::getAbsoluteTime();
                const double elapsedTime  = frameEndTime - frameStartTime;
                runningTime += elapsedTime;

                // Used for synchronization
                const double remainingTime = TARGET_FRAME_TIME_S - elapsedTime;
                const auto remainingTimeMs = static_cast<uint64_t>(remainingTime);
                if (_lockFrameRate && remainingTimeMs > 0)
                {
                    platform::sleep(remainingTimeMs);
                }

                frameCount++;

                // NOTE: State copy/update should be done before handling input
                input::update(static_cast<float>(deltaTime));
                // log::info(std::format("Running Time: {}\tFrame Time: {}", runningTime, _clock.elapsedTime));
            }
        }

        // Unregister events
        unregisterEvent(static_cast<EventCode>(SystemEventCode::APPLICATION_QUIT), &_appEventListener);
        unregisterEvent(static_cast<EventCode>(SystemEventCode::KEY_PRESSED), &_keyEventListener);
        unregisterEvent(static_cast<EventCode>(SystemEventCode::KEY_RELEASED), &_keyEventListener);

        event::shutdown();
        input::shutdown();
        _renderer->shutdown();
        _platform.shutdown();

        return true;
    }
} // namespace tempest::core