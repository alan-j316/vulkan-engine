#pragma once
/**
 * @file VulkanBuffer.h
 * @author Alan Abraham P Kochumon
 * @date Created on: October 30, 2026
 *
 * @brief Vulkan Buffer Abstraction.
 *
 * @copyright Copyright (c) 2026 Alan Abraham P Kochumon
 */

#include "VulkanDevice.h"
#include "renderer/vulkan/VulkanTypes.inl"

namespace tempest::renderer
{
    struct BufferConfig
    {
        size_t size;
        vk::BufferUsageFlags usageFlags;
        vk::MemoryPropertyFlags property;
        vk::SharingMode sharingMode;
        Vector<uint32_t> queueFamilies; // Must be specified if sharing mode is not exclusive.
    };

    class VulkanBuffer
    {
    public:
        bool create(const VulkanDevice& device, const BufferConfig& config) noexcept;

        /**
         * Map the memory.
         * @note Must only be called by Host Visible Buffers.
         * @return A raw pointer to the memory.
         */
        void* map(size_t offset, size_t size);


        /**
         * Unmap the mapped memory.
         */
        void unmap();


        /**
         * Get the underlying vulkan buffer.
         */
        [[nodiscard]] vk::Buffer getBuffer() const noexcept { return *_buffer; }

    private:
        uint32_t findMemoryType(const VulkanDevice& device, uint32_t typeFilter,
                                vk::MemoryPropertyFlags flags) const noexcept;


    private:
        vk::raii::Buffer _buffer{ nullptr };
        vk::raii::DeviceMemory _memory{ nullptr };
        vk::MemoryPropertyFlags _propertyFlags;
        bool _mapped{ false };
    };
} // namespace tempest::renderer