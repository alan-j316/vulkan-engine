#pragma once
/**
 * @file VulkanBackend.h
 * @author Alan Abraham P Kochumon
 * @date Created on: October 22, 2026
 *
 * @brief Vulkan Backend implementation.
 *
 * @copyright Copyright (c) 2026 Alan Abraham P Kochumon
 */

#include "containers/Vector.h"
#include "renderer/RendererBackend.h"

#include <vector>


namespace tempest::renderer
{
    class VulkanBackend: public RendererBackend
    {
    public:
        bool init(platform::PlatformState* state, std::string_view applicationName,
                  RendererEventQueue* rendererEventQueue) noexcept override;
        void shutdown() noexcept override;
        void onResize(uint32_t width, uint32_t height) noexcept override;
        bool beginFrame(float deltaTime) noexcept override;
        bool endFrame(float deltaTime) noexcept override;
        Texture createTexture(std::string name, uint32_t width, uint32_t height, uint32_t channelCount,
                              const uint8_t* pixels, bool hasTransparency, bool autoRelease) noexcept override;
        void destroyTexture(const Texture& texture) noexcept override;

        /// Update the renderer's global state.
        bool updateGlobalState(flcn::Mat4F projection, flcn::Mat4F view, flcn::Vec3F viewPosition,
                               flcn::Vec4F ambientColor, uint32_t mode) noexcept override;
        bool updateObjectState(const GeometryRenderData& renderData) noexcept override;

    private:
        Vector<const char*> getRequiredExtensions() const noexcept;
        void createGraphicsCommandPoolAndAllocateBuffers() const noexcept;
        void setupDebugMessenger() const noexcept;
        void update() noexcept;

    private:
        struct VulkanContext* _ctx{};
    };
} // namespace tempest::renderer