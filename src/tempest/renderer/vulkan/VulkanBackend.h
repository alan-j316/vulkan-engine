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
        bool init(platform::PlatformState* state, std::string_view applicationName) noexcept override;
        void shutdown() noexcept override;
        void onResize(uint32_t width, uint32_t height) noexcept override;
        bool beginFrame(float deltaTime) noexcept override;
        bool endFrame(float deltaTime) noexcept override;

    private:
        Vector<const char*> getRequiredExtensions() const noexcept;
    };
} // namespace tempest::renderer