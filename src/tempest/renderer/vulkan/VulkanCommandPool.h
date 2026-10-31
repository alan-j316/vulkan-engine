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


#include "VulkanBuffer.h"
#include "VulkanGraphicsPipeline.h"
#include "core/Asserts.h"
#include "renderer/vulkan/VulkanTypes.inl"


namespace tempest::renderer
{
    /// Forward Declaration
    class VulkanDevice;

    class VulkanCommandPool
    {
    public:
        /// Create a command pool. Must be called before allocating a command buffer.
        bool createCommandPool(const VulkanDevice& device, uint32_t queueFamilyIndex) noexcept;

        /// Destroy the command pool
        void destroy() noexcept;

        /// Allocate a new command buffer from the command pool.
        CommandBufferHandle allocate(const VulkanDevice& device, bool isPrimary);

        /// Free the command buffer from the command pool.
        void free(CommandBufferHandle handle);

        /// Start recording to the command buffer.
        void startRecording(CommandBufferHandle handle, bool isSingleUse, bool isSimultaneousUse) noexcept;

        /// End command buffer recording.
        void endRecording(CommandBufferHandle handle) noexcept;

        /// Submit the command buffer with the given @p handle.
        void submit(CommandBufferHandle handle) noexcept;

        /// Resets the command buffer
        void reset(CommandBufferHandle handle) noexcept;

        /// Allocate a single time command buffer and start recording.
        CommandBufferHandle allocateAndStartSingleTimeRecording(const VulkanDevice& device) noexcept;

        /// Ends Single Time Command Buffer Recording.
        void endSingleTimeRecordingAndSubmit(CommandBufferHandle handle, const vk::raii::Queue& queue) noexcept;


        /// +---------------------------------------------------------------------------------------+
        /// |                          FUNCTION PASSTHROUGHS                                        |
        /// +---------------------------------------------------------------------------------------+

        /// Bind a pipeline to a given command buffer.
        inline void bindPipeline(const CommandBufferHandle handle, const vk::PipelineBindPoint bindPoint,
                                 const VulkanGraphicsPipeline& pipeline) const noexcept
        { _commandBuffers[handle].bindPipeline(bindPoint, *pipeline.getBasePipeline()); }


        /// Execute the given command buffer's draw command.
        inline void draw(const CommandBufferHandle handle, const uint32_t vertexCount, const uint32_t instanceCount,
                         const uint32_t firstVertex, const uint32_t firstInstance) const noexcept
        { _commandBuffers[handle].draw(vertexCount, instanceCount, firstVertex, firstInstance); }


        /// Binds the vertex buffer to the given command buffer.
        inline void bindVertexBuffer(const CommandBufferHandle handle, const size_t bindingIndex,
                                     const VulkanBuffer& buffer) const
        { _commandBuffers[handle].bindVertexBuffers(static_cast<uint32_t>(bindingIndex), buffer.buffer(), { 0 }); }


        // TODO: Update to standard vk type rather than raii
        [[nodiscard]] const vk::raii::CommandBuffer& getBaseCommandBuffer(
            const CommandBufferHandle handle) const noexcept
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