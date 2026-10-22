#pragma once
/**
 * @file RendererBackend.h
 * @author Alan Abraham P Kochumon
 * @date Created on: October 22, 2026
 *
 * @brief Renderer Backend.
 *
 * @copyright Copyright (c) 2026 Alan Abraham P Kochumon
 */

#include "renderer/RendererTypes.inl"

namespace tempest::renderer
{
    class RendererBackend
    {
    public:
        virtual bool init(platform::PlatformState* _state, std::string_view applicationName) noexcept = 0;
        virtual void shutdown() noexcept                                                              = 0;
        virtual void onResize(uint32_t width, uint32_t height) noexcept                               = 0;
        virtual bool beginFrame(float deltaTime) noexcept                                             = 0;
        virtual bool endFrame(float deltaTime) noexcept                                               = 0;

        constexpr void incrementFrameNumber() noexcept { ++_frameNumber; }
        constexpr size_t getFrameNumber() const noexcept { return _frameNumber; }

        virtual ~RendererBackend() = default;

    protected:
        platform::PlatformState* _state{};
        size_t _frameNumber{};
    };

} // namespace tempest::renderer