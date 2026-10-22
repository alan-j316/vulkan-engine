#pragma once
/**
 * @file VulkanTypes.inl
 * @author Alan Abraham P Kochumon
 * @date Created on: October 22, 2026
 *
 * @brief Vulkan Structures and Types.
 *
 * @copyright Copyright (c) 2026 Alan Abraham P Kochumon
 */


// Disables Vulkan Constructor generation to support C++20 designated initializers
#define VULKAN_HPP_NO_CONSTRUCTORS

#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_raii.hpp>


namespace tempest::renderer
{

    struct VulkanContext
    {
        vk::Instance instance{};
        vk::raii::Context baseContext{};
        vk::AllocationCallbacks allocator{};
    };


    static constexpr auto MIN_VULKAN_VERSION = VK_API_VERSION_1_3;

} // namespace tempest::renderer