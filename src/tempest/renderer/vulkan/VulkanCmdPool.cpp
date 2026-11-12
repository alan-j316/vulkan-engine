/**
 * @file VulkanCommandPool.cpp
 * @author Alan Abraham P Kochumon
 * @date Created on: September 29, 2026
 *
 * @brief Implementation of member functions declared in CommandBuffer.h
 *
 * @copyright Copyright (c) 2026 Alan Abraham P Kochumon
 */

#include "VulkanCmdPool.h"
#include "VulkanDevice.h"
#include "core/Asserts.h"


namespace tempest::renderer
{
    void VulkanCmdPool::makeActive(const CmdBufferHandle handle) noexcept { _activeHandle = handle; }

    bool VulkanCmdPool::createCommandPool(const VulkanDevice& device, const uint32_t queueFamilyIndex) noexcept
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
        _cmdPool = vk::raii::CommandPool(device.getLogicalDevice(), commandPoolCreateInfo);
        return _cmdPool != nullptr;
    }


    void VulkanCmdPool::destroy() noexcept
    {
        _cmdBuffers.clear();
        _freeHandles.clear();
        _states.clear();
        _cmdPool = nullptr;
    }


    VulkanCmdPool::CmdBufferHandle VulkanCmdPool::allocate(const VulkanDevice& device, const bool isPrimary)
    {
        TEMPEST_ASSERT(
            _cmdPool != nullptr,
            "[CommandBuffer Allocate]: Command Pool not created! Call createCommandPool(...) to create a command "
            "pool before allocating buffers.");

        const vk::CommandBufferAllocateInfo allocateInfo{
            .commandPool        = _cmdPool,
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
            _states[handle]         = CommandBufferState::UNALLOCATED;
            _cmdBuffers[handle] = std::move(vk::raii::CommandBuffers(device.getLogicalDevice(), allocateInfo)[0]);
            return handle;
        }
        else
        {
            const auto handle = _cmdBuffers.size();
            TEMPEST_ASSERT(_cmdBuffers.size() == _states.size(), "Command buffer and states out-of-sync");
            _states.push_back(CommandBufferState::UNALLOCATED);
            _cmdBuffers.push_back(std::move(vk::raii::CommandBuffers(device.getLogicalDevice(), allocateInfo)[0]));
            return handle;
        }
    }


    void VulkanCmdPool::free(const CmdBufferHandle handle)
    {
        _freeHandles.push_back(handle);
        _states[handle] = CommandBufferState::FREED;
    }


    void VulkanCmdPool::freeActive() { free(_activeHandle); }


    void VulkanCmdPool::startRecording(const bool isSingleUse, const bool isSimultaneousUse) noexcept
    {
        TEMPEST_ASSERT(_activeHandle != InvalidCmdBufferHandle,
                       "[CmdBuffer]: No active handle. Set one using setActive.");
        TEMPEST_ASSERT(_states[_activeHandle] != CommandBufferState::FREED,
                       "[Command Buffer Start Recording]: Trying to record to a freed command buffer");

        vk::CommandBufferUsageFlags flags = static_cast<vk::CommandBufferUsageFlagBits>(0x0);
        flags |= isSingleUse ? vk::CommandBufferUsageFlagBits::eOneTimeSubmit : flags;
        flags |= isSimultaneousUse ? vk::CommandBufferUsageFlagBits::eSimultaneousUse : flags;

        const vk::CommandBufferBeginInfo beginInfo{ .flags = flags };
        _cmdBuffers[_activeHandle].begin(beginInfo);
        _states[_activeHandle] = CommandBufferState::RECORDING;
    }


    void VulkanCmdPool::endRecording() noexcept
    {
        TEMPEST_ASSERT(_activeHandle != InvalidCmdBufferHandle,
                       "[CmdBuffer]: No active handle. Set one using setActive.");
        TEMPEST_ASSERT(_states[_activeHandle] != CommandBufferState::FREED,
                       "[Command Buffer End Recording]: Trying to end recording on a freed command buffer");
        _states[_activeHandle] = CommandBufferState::RECORDING_ENDED;
        _cmdBuffers[_activeHandle].end();
    }


    void VulkanCmdPool::submit() noexcept
    {
        TEMPEST_ASSERT(_activeHandle != InvalidCmdBufferHandle,
                       "[CmdBuffer]: No active handle. Set one using setActive.");
        TEMPEST_ASSERT(_states[_activeHandle] != CommandBufferState::FREED,
                       "[Command Buffer Submit]: Trying to submit on a freed command buffer");
        _states[_activeHandle] = CommandBufferState::SUBMITTED;
    }


    void VulkanCmdPool::reset() noexcept
    {
        TEMPEST_ASSERT(_states[_activeHandle] != CommandBufferState::FREED,
                       "[Command Buffer Reset]: Trying to reset on a freed command buffer");
        _states[_activeHandle] = CommandBufferState::READY;
        _cmdBuffers[_activeHandle].reset();
    }


    VulkanCmdPool::CmdBufferHandle VulkanCmdPool::allocateAndStartSingleTimeRecording(const VulkanDevice& device) noexcept
    {
        const auto handle = allocate(device, true);
        makeActive(handle);
        startRecording(true, false);
        return handle;
    }


    void VulkanCmdPool::endSingleTimeRecordingAndSubmit(const CmdBufferHandle handle,
                                                            const vk::raii::Queue& queue) noexcept
    {
        makeActive(handle);
        endRecording();

        // Submit to the queue
        const vk::SubmitInfo submitInfo{
            .commandBufferCount = 1,
            .pCommandBuffers    = &*_cmdBuffers[handle],
        };

        queue.submit(submitInfo);
        // Since we are not use a fence we need to wait for the submission to end
        queue.waitIdle(); // TODO: Add fence

        // Free the command buffer
        free(handle);
    }

} // namespace tempest::renderer
