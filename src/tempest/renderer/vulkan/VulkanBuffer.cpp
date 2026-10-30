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
    bool VulkanBuffer::create(const VulkanDevice& device, const BufferConfig& config) noexcept
    {
        _propertyFlags = config.property;

        // Create the buffer
        // TODO: Add queue family index for sharing.
        const vk::BufferCreateInfo bufferInfo{ .size        = config.size,
                                               .usage       = config.usageFlags,
                                               .sharingMode = config.sharingMode,
                                               .queueFamilyIndexCount =
                                                   static_cast<uint32_t>(config.queueFamilies.size()),
                                               .pQueueFamilyIndices = config.queueFamilies.data() };
        // TODO: Update with vulkan allocator
        _buffer = vk::raii::Buffer(device.getLogicalDevice(), bufferInfo, nullptr);

        // Query memory requirements and find the heap type index
        const vk::MemoryRequirements memoryRequirements = _buffer.getMemoryRequirements();
        const auto type = findMemoryType(device, memoryRequirements.memoryTypeBits, config.property);

        // Allocate device memory
        const vk::MemoryAllocateInfo allocateInfo{
            .allocationSize  = memoryRequirements.size,
            .memoryTypeIndex = type,
        };

        // TODO: Update with custom allocator
        _memory = vk::raii::DeviceMemory(device.getLogicalDevice(), allocateInfo, nullptr);

        // Bind the memory
        _buffer.bindMemory(_memory, 0);

        return _buffer != nullptr && _memory != nullptr;
    }


    void* VulkanBuffer::map(const size_t offset, const size_t size)
    {
        TEMPEST_ASSERT((_propertyFlags & vk::MemoryPropertyFlagBits::eHostVisible) ==
                           vk::MemoryPropertyFlagBits::eHostVisible,
                       "Cannot map non-host memory");
        _mapped = true;
        return _memory.mapMemory(offset, size);
    }


    void VulkanBuffer::unmap()
    {
        const std::source_location location = std::source_location::current();
        TEMPEST_ASSERT(_mapped == true,
                       std::format("Cannot unmapped memory that is not mapped yet!\nFile: {} Line: {}:{}",
                                   location.file_name(), location.line(), location.column())
                           .c_str());
        _memory.unmapMemory();
        _mapped = false;
    }


    uint32_t VulkanBuffer::findMemoryType(const VulkanDevice& device, const uint32_t typeFilter,
                                          const vk::MemoryPropertyFlags flags) const noexcept
    {
        const auto memoryProperties = device.getPhysicalDevice().getMemoryProperties();

        for (size_t i{ 0 }; i < memoryProperties.memoryTypeCount; ++i)
        {
            if ((typeFilter & (1 << i)) && (memoryProperties.memoryTypes[i].propertyFlags & flags) == flags)
            {
                return i;
            }
        }

        log::error("Unable to find the memory type!");
        return std::numeric_limits<uint32_t>::max();
    }
} // namespace tempest::renderer