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
        bool create(const vk::raii::Instance& instance, const vk::raii::SurfaceKHR& surface,
                    const PhysicalDeviceRequirements& physicalDeviceRequirements);
        void destroy();
        static void querySwapChainSupport(const vk::raii::PhysicalDevice& device, const vk::SurfaceKHR& surface,
                                          VulkanSwapChainSupportInfo& info) noexcept;
        [[nodiscard]] uint32_t findMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags properties) const noexcept;
        bool detectDepthFormat() noexcept;

        void setDepthFormat(const vk::Format format) noexcept { _depthFormat = format; }


        [[nodiscard]] vk::Format getDepthFormat() const noexcept { return _depthFormat; }
        [[nodiscard]] constexpr const vk::raii::PhysicalDevice& getPhysicalDevice() const noexcept
        { return _physicalDevice; }
        [[nodiscard]] constexpr const vk::raii::Device& getLogicalDevice() const noexcept { return _logicalDevice; }

        /**
         * @note Don't use for querying capabilities as it can give outdated values, esp the window size.
         *       Use @ref queryCapabilities method directly.
         */
        [[nodiscard]] constexpr const VulkanSwapChainSupportInfo& getSwapChainSupport() const noexcept
        {
            // TODO: Refactor, swapchain support need not be cache as the window resize can persist the
            // window size and cause crashes
            return _swapChainSupport;
        }

        [[nodiscard]] vk::SurfaceCapabilitiesKHR getSurfaceCapabilities(
            const vk::raii::SurfaceKHR& surface) const noexcept;

        [[nodiscard]] constexpr const VulkanQueueFamilyInfo& getQueueInfo() const noexcept { return _queueInfo; }

        [[nodiscard]] constexpr const vk::PhysicalDeviceProperties& getProperties() const noexcept
        { return _properties; }
        [[nodiscard]] constexpr const vk::PhysicalDeviceFeatures& getFeatures() const noexcept { return _features; }
        [[nodiscard]] constexpr const vk::PhysicalDeviceMemoryProperties& getMemoryProperties() const noexcept
        { return _memory; }

        [[nodiscard]] constexpr vk::raii::Queue& getGraphicsQueue() noexcept { return _graphicsQueue; }
        [[nodiscard]] constexpr vk::raii::Queue& getPresentQueue() noexcept { return _presentQueue; }
        [[nodiscard]] constexpr vk::raii::Queue& getTransferQueue() noexcept { return _transferQueue; }

    private:
        bool pickPhysicalDevice(const vk::raii::Instance& instance, const vk::raii::SurfaceKHR& surface,
                                const PhysicalDeviceRequirements& requirements) noexcept;

    private:
        vk::raii::PhysicalDevice _physicalDevice{ nullptr };
        vk::raii::Device _logicalDevice{ nullptr };
        VulkanSwapChainSupportInfo _swapChainSupport{};
        VulkanQueueFamilyInfo _queueInfo{};
        vk::PhysicalDeviceProperties _properties{};
        vk::PhysicalDeviceFeatures _features{};
        vk::PhysicalDeviceMemoryProperties _memory{};
        // TODO: Add compute queue
        vk::raii::Queue _graphicsQueue{ nullptr }, _presentQueue{ nullptr }, _transferQueue{ nullptr };
        float _queuePriorities[3]{ 1.0f, 1.0f, 1.0f };
        vk::Format _depthFormat{};

    public:
    };
} // namespace tempest::renderer