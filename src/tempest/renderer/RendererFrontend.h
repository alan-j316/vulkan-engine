#pragma once
/**
 * @file RendererFrontend.h
 * @author Alan Abraham P Kochumon
 * @date Created on: October 22, 2026
 *
 * @brief Renderer Frontend.
 *
 * @copyright Copyright (c) 2026 Alan Abraham P Kochumon
 */

#include "events/RendererEventQueue.h"
#include "renderer/RendererTypes.inl"

#include <cstdint>
#include <resources/ResourceTypes.h>
#include <string>

namespace tempest::renderer
{
    class RendererFrontend
    {
    public:
        bool init(RendererType type, std::string_view applicationName, platform::PlatformState* platformState,
                  RendererEventQueue* rendererEventQueue) noexcept;
        void shutdown() noexcept;
        void onResize(uint32_t width, uint32_t height) noexcept;
        bool drawFrame(const RenderPacket& packet) const noexcept;
        Texture createTexture(std::string name, uint32_t width, uint32_t height, uint32_t channelCount,
                              const uint8_t* pixels, bool hasTransparency, bool autoRelease) const noexcept;
        void destroyTexture(const Texture& texture) const noexcept;

        /// +---------------------------------------------------------------------------------------+
        /// |                              SETTERS/GETTERS                                          |
        /// +---------------------------------------------------------------------------------------+

        constexpr void setView(const flcn::Mat4F& view) { _view = view; }


    private:
        bool beginFrame(float deltaTime) const noexcept;
        bool endFrame(float deltaTime) const noexcept;

    private:
        class RendererBackend* _rendererBackend{};
        RendererType _activeBackend{ RendererType::INVALID_BACKEND };
        flcn::Mat4F _projection{}, _view{};
        float _farClip{ 0.01f }, _nearClip{ 10.0f };
    };

} // namespace tempest::renderer
