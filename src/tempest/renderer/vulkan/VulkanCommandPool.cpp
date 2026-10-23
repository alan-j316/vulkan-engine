/**
 * @file VulkanCommandPool.cpp
 * @author Alan Abraham P Kochumon
 * @date Created on: September 29, 2026
 *
 * @brief Implementation of member functions declared in CommandBuffer.h
 *
 * @copyright Copyright (c) 2026 Alan Abraham P Kochumon
 */

#include "VulkanCommandPool.h"

#include "VulkanDevice.h"
#include "core/Asserts.h"


namespace tempest::renderer
{


    bool VulkanCommandPool::createCommandPool(const VulkanContext& context, const uint32_t queueFamilyIndex) noexcept
    {
        // For allocation command buffer we need a command pool first
        const vk::CommandPoolCreateInfo commandPoolCreateInfo{
            // Allow individual re-recording
            // Transient-> Allows command buffers to be rerecorded with new commands often.
            .flags            = vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
            .queueFamilyIndex = queueFamilyIndex
        };

        // Command buffers execute by submitting them to ONE of the device queues, like graphics or presentation,
        // and each queue type require a different command buffer
        _commandPool = vk::raii::CommandPool(context.device->getLogicalDevice(), commandPoolCreateInfo);
        return _commandPool != nullptr;
    }

    void VulkanCommandPool::destroy() noexcept
    {
        _commandBuffers.clear();
        _freeHandles.clear();
        _states.clear();
        _commandPool = nullptr;
    }


    CommandBufferHandle VulkanCommandPool::allocate(const VulkanContext& context, bool isPrimary)
    {
        // TODO: Create command pool
        TEMPEST_ASSERT(
            _commandPool != nullptr,
            "[CommandBuffer Allocate]: Command Pool not created! Call createCommandPool(...) to create a command "
            "pool before allocating buffers.");

        const vk::CommandBufferAllocateInfo allocateInfo{
            .commandPool        = _commandPool,
            .level              = isPrimary ? vk::CommandBufferLevel::ePrimary : vk::CommandBufferLevel::eSecondary,
            .commandBufferCount = 1,
        };

        // TODO: Update handles to use indirect handles from a handle pool
        //       and use pop and swap
        // Reuse command buffer slots
        if (_freeHandles.size() > 0)
        {
            const auto handle = _freeHandles.size() - 1;
            _freeHandles.pop_back();
            _states[handle] = CommandBufferState::UNALLOCATED;
            _commandBuffers[handle] =
                std::move(vk::raii::CommandBuffers(context.device->getLogicalDevice(), allocateInfo)[0]);
            return handle;
        }
        else
        {
            const auto handle = _commandBuffers.size();
            TEMPEST_ASSERT(_commandBuffers.size() == _states.size(), "Command buffer and states out-of-sync");
            _states.push_back(CommandBufferState::UNALLOCATED);
            _commandBuffers.push_back(
                std::move(vk::raii::CommandBuffers(context.device->getLogicalDevice(), allocateInfo)[0]));
            return handle;
        }
    }


    void VulkanCommandPool::free(const VulkanContext& context, const CommandBufferHandle handle)
    {
        _freeHandles.push_back(handle);
        _states[handle] = CommandBufferState::FREED;
    }


    void VulkanCommandPool::startRecording(const CommandBufferHandle handle, const bool isSingleUse,
                                           const bool isSimultaneousUse) noexcept
    {
        TEMPEST_ASSERT(_states[handle] != CommandBufferState::FREED,
                       "[Command Buffer Start Recording]: Trying to record to a freed command buffer");

        vk::CommandBufferUsageFlags flags = static_cast<vk::CommandBufferUsageFlagBits>(0x0);
        flags |= isSingleUse ? vk::CommandBufferUsageFlagBits::eOneTimeSubmit : flags;
        flags |= isSimultaneousUse ? vk::CommandBufferUsageFlagBits::eSimultaneousUse : flags;

        const vk::CommandBufferBeginInfo beginInfo{ .flags = flags };
        _commandBuffers[handle].begin(beginInfo);
        _states[handle] = CommandBufferState::RECORDING;
    }


    void VulkanCommandPool::endRecording(const CommandBufferHandle handle) noexcept
    {
        TEMPEST_ASSERT(_states[handle] != CommandBufferState::FREED,
                       "[Command Buffer End Recording]: Trying to end recording on a freed command buffer");
        _states[handle] = CommandBufferState::RECORDING_ENDED;
        _commandBuffers[handle].end();
    }


    void VulkanCommandPool::submit(const CommandBufferHandle handle) noexcept
    {
        TEMPEST_ASSERT(_states[handle] != CommandBufferState::FREED,
                       "[Command Buffer Submit]: Trying to submit on a freed command buffer");
        _states[handle] = CommandBufferState::SUBMITTED;
    }


    void VulkanCommandPool::reset(const CommandBufferHandle handle) noexcept
    {
        TEMPEST_ASSERT(_states[handle] != CommandBufferState::FREED,
                       "[Command Buffer Reset]: Trying to reset on a freed command buffer");
        _states[handle] = CommandBufferState::READY;
        _commandBuffers[handle].reset();
    }

    CommandBufferHandle VulkanCommandPool::allocateAndStartSingleTimeRecording(const VulkanContext& context) noexcept
    {
        const auto handle = allocate(context, true);
        startRecording(handle, true, false);
        return handle;
    }


    void VulkanCommandPool::endSingleTimeRecordingAndSubmit(const VulkanContext& context,
                                                            const CommandBufferHandle handle,
                                                            const vk::raii::Queue& queue) noexcept
    {
        endRecording(handle);

        // Submit to the queue
        vk::SubmitInfo submitInfo{
            .commandBufferCount = 1,
            .pCommandBuffers    = &*_commandBuffers[handle],
        };

        queue.submit(submitInfo);
        // Since we are not use a fence we need to wait for the submission to end
        queue.waitIdle();

        // Free the command buffer
        free(context, handle);
    }

} // namespace tempest::renderer
