/**
 * @file VulkanBackend.cpp
 * @author Alan Abraham P Kochumon
 * @date Created on: October 22, 2026
 *
 * @brief Vulkan Backend member function implementation.
 *
 * @copyright Copyright (c) 2026 Alan Abraham P Kochumon
 */

#include "VulkanBackend.h"


namespace tempest::renderer
{

    bool VulkanBackend::init(platform::PlatformState* _state, std::string_view applicationName) noexcept
    { return true; }

    void VulkanBackend::shutdown() noexcept
    {
        // TODO: Impl
    }


    void VulkanBackend::onResize(uint32_t width, uint32_t height) noexcept
    {
        // TODO: Impl
    }


    bool VulkanBackend::beginFrame(float deltaTime) noexcept
    {
        // TODO: Impl
        return true;
    }


    bool VulkanBackend::endFrame(float deltaTime) noexcept
    {
        // TODO: Impl
        return true;
    }


} // namespace tempest::renderer