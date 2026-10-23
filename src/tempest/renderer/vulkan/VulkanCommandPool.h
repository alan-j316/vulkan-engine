#pragma once
/**
 * @file VulkanCommandPool.h
 * @author Alan Abraham P Kochumon
 * @date Created on: October 23, 2026
 *
 * @brief Command Buffer abstraction.
 *
 * @copyright Copyright (c) 2026 Alan Abraham P Kochumon
 */


#include "core/Asserts.h"
#include "renderer/vulkan/VulkanTypes.inl"


namespace tempest::renderer
{
    class VulkanCommandPool
    {
    public:
        /// Create a command pool. Must be called before allocating a command buffer.
        bool createCommandPool(const VulkanContext& context, uint32_t queueFamilyIndex) noexcept;

        /// Destroy the command pool
        void destroy() noexcept;

        /// Allocate a new command buffer from the command pool.
        CommandBufferHandle allocate(const VulkanContext& context, bool isPrimary);

        /// Free the command buffer from the command pool.
        void free(const VulkanContext& context, CommandBufferHandle handle);

        /// Start recording to the command buffer.
        void startRecording(CommandBufferHandle handle, bool isSingleUse, bool isSimultaneousUse) noexcept;

        /// End command buffer recording.
        void endRecording(CommandBufferHandle handle) noexcept;

        /// Submit the command buffer with the given @p handle.
        void submit(CommandBufferHandle handle) noexcept;

        /// Resets the command buffer
        void reset(CommandBufferHandle handle) noexcept;

        /// Allocate a single time command buffer and start recording.
        CommandBufferHandle allocateAndStartSingleTimeRecording(const VulkanContext& context) noexcept;

        /// Ends Single Time Command Buffer Recording.
        void endSingleTimeRecordingAndSubmit(const VulkanContext& context, CommandBufferHandle handle,
                                             const vk::raii::Queue& queue) noexcept;

        [[nodiscard]] vk::raii::CommandBuffer& getBaseCommandBuffer(const CommandBufferHandle handle) noexcept
        {
            TEMPEST_ASSERT(_states[handle] != CommandBufferState::FREED, "Trying to acquired a freed command buffer.");
            return _commandBuffers[handle];
        }

        [[nodiscard]] size_t getCommandBufferCount() const noexcept { return _commandBuffers.size(); }

    private:
        vk::raii::CommandPool _commandPool{ nullptr };
        Vector<vk::raii::CommandBuffer> _commandBuffers{ makeVector<vk::raii::CommandBuffer>() };
        Vector<CommandBufferHandle> _freeHandles{ makeVector<CommandBufferHandle>() };
        Vector<CommandBufferState> _states{ makeVector<CommandBufferState>() };
    };
} // namespace tempest::renderer