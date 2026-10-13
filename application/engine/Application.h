#pragma once
/**
 * @file Application.h
 * @author Alan Abraham P Kochumon
 * @date Created on: September 23, 2026
 *
 * @brief Game engine application.
 *
 * @copyright Copyright (c) 2026 Alan Abraham P Kochumon
 */

#include "platform/TempestWindow.h"
#include "renderer/TempestRenderer.h"

namespace tempest
{
    using namespace platform;

    class TempestApp
    {
    public:
        explicit TempestApp(std::string name, uint32_t version) noexcept;

        bool init() noexcept;

        void run() noexcept;

        static constexpr size_t INIT_WIDTH       = 800;
        static constexpr size_t INIT_HEIGHT      = 600;
        static constexpr auto ENGINE_NAME        = "Tempest";
        static constexpr uint32_t ENGINE_VERSION = 1;

    private:
        void handleEvents() noexcept;
        void loadModel(std::string_view modelPath);

        /// Member Variables
    private:
        TempestWindow _window;
        renderer::TempestRenderer _renderer;

        std::string _appName;
        uint32_t _appVersion;
        std::vector<Vertex> _vertices;
        std::vector<uint32_t> _indices;
        static constexpr auto MODEL_PATH   = "models/viking_room.obj";
        static constexpr auto TEXTURE_PATH = "textures/viking_room.png";

        bool _isRunning;
    };
} // namespace tempest