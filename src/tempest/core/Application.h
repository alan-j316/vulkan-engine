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
    namespace renderer
    {
        class RendererFrontend;
    }

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
            bool _lockFrameRate{ false };
            renderer::RendererFrontend* _renderer{};

            ApplicationEventListener _appEventListener;
            KeyEventListener _keyEventListener;

            // Friend class
            friend ApplicationEventListener;
            friend KeyEventListener;

            // Static variables
            static constexpr double TARGET_FRAME_TIME_S{ 1.0 / 60.0 }; ///< Frame time in seconds.
        };
    } // namespace core
} // namespace tempest