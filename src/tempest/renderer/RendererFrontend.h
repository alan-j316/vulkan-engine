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

#include "renderer/RendererTypes.inl"

#include <cstdint>
#include <string>


namespace tempest::renderer
{
    class RendererFrontend
    {
    public:
        bool init(RendererType type, std::string_view applicationName, platform::PlatformState* platformState) noexcept;
        void shutdown() noexcept;
        void onResize(uint32_t width, uint32_t height) noexcept;
        bool drawFrame(const RenderPacket& packet) const noexcept;

    private:
        bool beginFrame(float deltaTime) const noexcept;
        bool endFrame(float deltaTime) const noexcept;

    private:
        class RendererBackend* _rendererBackend{};
        RendererType _activeBackend{ RendererType::INVALID_BACKEND };
    };

} // namespace tempest::renderer
