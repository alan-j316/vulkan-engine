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

#include <vk_mem_alloc.h>

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
        bool create(VmaAllocator vma, const BufferConfig& config) noexcept;

        /**
         * Map the memory.
         * @note Must only be called by Host Visible Buffers.
         *
         */
        void* map() const;


        /**
         * Unmap the mapped memory.
         */
        void unmap() const;

        void destroy() const noexcept;


        /**
         * Get the underlying vulkan buffer.
         */
        [[nodiscard]] vk::Buffer buffer() const noexcept { return _buffer; }

    private:
        size_t _size{};
        vk::Buffer _buffer{};
        VmaAllocation _allocation{};
        VmaAllocator _allocator{};
        void* _memory{};
    };
} // namespace tempest::renderer