/**
 * @file VulkanBuffer.cpp
 * @author Alan Abraham P Kochumon
 * @date Created on: October 30, 2026
 *
 * @brief Implementation of functions declared in VulkanBuffer.h
 *
 * @copyright Copyright (c) 2026 Alan Abraham P Kochumon
 */

#include "VulkanBuffer.h"

#include "core/Asserts.h"
#include "core/Logger.h"

#include <format>
#include <source_location>

namespace tempest::renderer
{
    bool VulkanBuffer::create(const VmaAllocator vma, const BufferConfig& config) noexcept
    {
        _allocator = vma;
        _size      = config.size;

        const VkBufferCreateInfo bufferInfo{ .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
                                             .size  = config.size,
                                             .usage = static_cast<VkBufferUsageFlags>(config.usageFlags) };

        // TODO: Update to a configuration that matches our needs based on config
        const VmaAllocationCreateInfo allocInfo{ .flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT,
                                                 .usage = VMA_MEMORY_USAGE_AUTO_PREFER_HOST,
                                                 .requiredFlags = VK_MEMORY_PROPERTY_HOST_COHERENT_BIT };

        VkBuffer buffer;
        if (vmaCreateBuffer(vma, &bufferInfo, &allocInfo, &buffer, &_allocation, nullptr) != VK_SUCCESS)
        {
            log::error("There was an error creating the buffer!");
            return false;
        }
        _buffer = buffer;

        return _buffer != nullptr;
    }

    void* VulkanBuffer::map() const
    {
        // TODO: May be remove the conditional?
        void* memory;
        if (vmaMapMemory(_allocator, _allocation, &memory) != VK_SUCCESS)
        {
            log::error("There was an error mapping the memory!");
        }

        return memory;
    }

    void VulkanBuffer::unmap() const { vmaUnmapMemory(_allocator, _allocation); }

    void VulkanBuffer::destroy() const noexcept { vmaDestroyBuffer(_allocator, _buffer, _allocation); }

} // namespace tempest::renderer