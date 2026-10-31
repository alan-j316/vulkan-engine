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
#include "AssetManager.h"
#include "Clock.h"
#include "core/EventListeners.h"
#include "platform/Platform.h"
#include "renderer/RendererFrontend.h"


namespace tempest
{
    /// Forward declaration
    class Game;
    namespace renderer
    {
        class RendererFrontend;
    }

    struct TempestContext
    {
        core::AssetManager assetManager;
    };

    namespace core
    {
        class TEMPEST_API Application
        {
        public:
            Application() noexcept;
            bool create() noexcept;
            bool run() noexcept;
            void onResize(uint32_t w, uint32_t h) const;

        private:
            bool _isRunning{};
            bool _isSuspended{};
            platform::Platform _platform{};
            Game* _game{};
            uint32_t _width{}, _height{};
            double _lastTime{};
            Clock _clock;
            bool _lockFrameRate{ false };
            renderer::RendererFrontend _renderer{};
            TempestContext _context{};

            ApplicationEventListener _appEventListener;
            KeyEventListener _keyEventListener;
            ResizeEventListener _resizeEventListener;

            // Friend class
            friend ApplicationEventListener;
            friend KeyEventListener;
            friend ResizeEventListener;

            // Static variables
            static constexpr double TARGET_FRAME_TIME_S{ 1.0 / 60.0 }; ///< Frame time in seconds.
        };
    } // namespace core
} // namespace tempest

extern "C"
{
    tempest::Game* createGame(tempest::TempestContext& ctx);
    bool destroyGame(tempest::Game* game, tempest::TempestContext& ctx);
}
