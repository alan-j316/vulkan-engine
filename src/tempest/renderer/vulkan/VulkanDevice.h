#pragma once
/**
 * @file Device.h
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
        bool create(const VulkanContext& context);
        void destroy(const VulkanContext& context);

    private:
        vk::raii::PhysicalDevice physicalDevice{ nullptr };
        vk::raii::Device logicalDevice{ nullptr };
    };
} // namespace tempest::renderer