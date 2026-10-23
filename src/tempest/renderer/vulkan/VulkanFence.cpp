/**
 * @file VulkanFence.cpp
 * @author Alan Abraham P Kochumon
 * @date Created on: October 23, 2026
 *
 * @brief Implmentation of member functions declared in VulkanFence.h
 *
 * @copyright Copyright (c) 2026 Alan Abraham P Kochumon
 */

#include "VulkanFence.h"

#include "core/Logger.h"
#include "renderer/vulkan/VulkanDevice.h"

namespace tempest::renderer
{

    void VulkanFence::create(const VulkanContext& context, const bool createSignaled)
    {
        _isSignaled = createSignaled;
        const vk::FenceCreateFlags flags =
            createSignaled ? vk::FenceCreateFlagBits::eSignaled : static_cast<vk::FenceCreateFlagBits>(0x0);
        const vk::FenceCreateInfo createInfo{ .flags = flags };

        _fence = vk::raii::Fence(context.device->getLogicalDevice(), createInfo, context.allocator);
    }


    void VulkanFence::destroy(const VulkanContext& context) noexcept
    {
        _fence      = nullptr;
        _isSignaled = false;
        // RAII will do the rest of the deallocation.
    }

    bool VulkanFence::wait(const VulkanContext& context, const size_t timeout) noexcept
    {
        if (_isSignaled)
            return true;
        const auto result = context.device->getLogicalDevice().waitForFences(*_fence, true, timeout);
        switch (result)
        {
            case vk::Result::eSuccess:
                _isSignaled = true;
                return true;
            case vk::Result::eTimeout:
                log::warn("Vulkan Fence Wait - Timeout");
                break;
            case vk::Result::eErrorDeviceLost:
                log::error("Vulkan Fence Wait - Device Lost/Disconnected!");
                break;
            case vk::Result::eErrorOutOfHostMemory:
                log::error("Vulkan Fence Wait - Out of Host Memory!");
                break;
            case vk::Result::eErrorOutOfDeviceMemory:
                log::error("Vulkan Fence Wait - Out of Device Memory!");
                break;
            default:
                log::error("Vulkan Fence Wait - Unknown Wait Error!");
                break;
        }
        return false;
    }


    void VulkanFence::reset(const VulkanContext& context) noexcept
    {
        if (_isSignaled)
        {
            context.device->getLogicalDevice().resetFences(*_fence);
            _isSignaled = false;
        }
    }

} // namespace tempest::renderer