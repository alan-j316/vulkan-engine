#pragma once
/**
 * @file VulkanDevice.h
 * @author Alan Abraham P Kochumon
 * @date Created on: October 22, 2026
 *
 * @brief Vulkan Device.
 *
 * @copyright Copyright (c) 2026 Alan Abraham P Kochumon
 */

#include "renderer/vulkan/VulkanTypes.inl"

namespace tempest::renderer
{
    class VulkanDevice
    {
    public:
        bool create(const VulkanContext& context, const PhysicalDeviceRequirements& physicalDeviceRequirements);
        void destroy(const VulkanContext& context);
        static void querySwapChainSupport(const vk::raii::PhysicalDevice& device, const vk::SurfaceKHR& surface,
                                          VulkanSwapChainSupportInfo& info) noexcept;

    private:
        inline bool pickPhysicalDevice(const VulkanContext& context,
                                       const PhysicalDeviceRequirements& requirements) noexcept;

    private:
        vk::raii::PhysicalDevice _physicalDevice{ nullptr };
        vk::raii::Device _logicalDevice{ nullptr };
        VulkanSwapChainSupportInfo _swapChainSupport{};
        VulkanQueueFamilyInfo _queueInfo{};
        vk::PhysicalDeviceProperties _properties{};
        vk::PhysicalDeviceFeatures _features{};
        vk::PhysicalDeviceMemoryProperties _memory{};
    };
} // namespace tempest::renderer