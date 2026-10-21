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
#include "Clock.h"
#include "core/EventListeners.h"
#include "platform/Platform.h"

namespace tempest
{
    /// Forward declaration
    class Game;

    namespace core
    {
        class TEMPEST_API Application
        {
        public:
            Application() noexcept;
            bool create(Game* game) noexcept;
            bool run() noexcept;

        private:
            bool _isRunning{};
            bool _isSuspended{};
            platform::Platform _platform{};
            Game* _game{};
            uint32_t _width{}, _height{};
            double _lastTime{};
            Clock _clock;

            ApplicationEventListener _appEventListener;
            KeyEventListener _keyEventListener;

            // Friend class
            friend ApplicationEventListener;
            friend KeyEventListener;

            // Static variables
            static constexpr double TARGET_FRAME_TIME_MS{ 1.0 / 60.0 };
        };
    } // namespace core
} // namespace tempest