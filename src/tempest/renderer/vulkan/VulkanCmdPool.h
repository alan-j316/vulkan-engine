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

    class VulkanCmdPool
    {
    public:
        /// +---------------------------------------------------------------------------------------+
        /// |                           ALIAS & PUBLIC VARS                                         |
        /// +---------------------------------------------------------------------------------------+
        /// Type aliases
        using CmdBufferHandle                                   = uint32_t;
        static constexpr CmdBufferHandle InvalidCmdBufferHandle = std::numeric_limits<CmdBufferHandle>::max();


        /// +---------------------------------------------------------------------------------------+
        /// |                             PUBLIC METHODS                                            |
        /// +---------------------------------------------------------------------------------------+

        /// Mark a command buffer as active. Must be called before an methods are invoked.
        void makeActive(CmdBufferHandle handle) noexcept;

        /// Create a command pool. Must be called before allocating a command buffer.
        bool createCommandPool(const VulkanDevice& device, uint32_t queueFamilyIndex) noexcept;

        /// Destroy the command pool
        void destroy() noexcept;

        /// Allocate a new command buffer from the command pool.
        CmdBufferHandle allocate(const VulkanDevice& device, bool isPrimary);

        /// Free the command buffer from the command pool.
        void free(CmdBufferHandle handle);

        /// Frees the active command buffer from the pool.
        void freeActive();

        /// Start recording to the command buffer.
        /// An active command buffer must be set using @ref setActive.
        void startRecording(bool isSingleUse, bool isSimultaneousUse) noexcept;

        /// End command buffer recording.
        /// An active command buffer must be set using @ref setActive.
        void endRecording() noexcept;

        /// Submit the command buffer with the given @p handle.
        /// An active command buffer must be set using @ref setActive.
        void submit() noexcept;

        /// Resets the command buffer
        /// An active command buffer must be set using @ref setActive.
        void reset() noexcept;

        /// Allocate a single time command buffer and start recording.
        CmdBufferHandle allocateAndStartSingleTimeRecording(const VulkanDevice& device) noexcept;

        /// Ends Single Time Command Buffer Recording.
        void endSingleTimeRecordingAndSubmit(CmdBufferHandle handle, const vk::raii::Queue& queue) noexcept;


        /// +---------------------------------------------------------------------------------------+
        /// |                          FUNCTION PASSTHROUGHS                                        |
        /// +---------------------------------------------------------------------------------------+

        /// Bind a pipeline to a given command buffer.
        inline void bindPipeline(const vk::PipelineBindPoint bindPoint,
                                 const VulkanGraphicsPipeline& pipeline) const noexcept
        {
            TEMPEST_ASSERT(_activeHandle != InvalidCmdBufferHandle,
                           "[CmdBuffer]: No active handle. Set one using setActive.");
            _cmdBuffers[_activeHandle].bindPipeline(bindPoint, *pipeline.getBasePipeline());
        }


        /// Execute the given command buffer's draw command.
        inline void draw(const uint32_t vertexCount, const uint32_t instanceCount, const uint32_t firstVertex,
                         const uint32_t firstInstance) const noexcept
        {
            TEMPEST_ASSERT(_activeHandle != InvalidCmdBufferHandle,
                           "[CmdBuffer]: No active handle. Set one using setActive.");
            _cmdBuffers[_activeHandle].draw(vertexCount, instanceCount, firstVertex, firstInstance);
        }


        /// Execute the given command buffer's draw indexed command.
        inline void drawIndexed(const uint32_t indexCount, const uint32_t instanceCount, const uint32_t firstIndex,
                                const int32_t vertexOffset, const uint32_t firstInstance) const
        {
            TEMPEST_ASSERT(_activeHandle != InvalidCmdBufferHandle,
                           "[CmdBuffer]: No active handle. Set one using setActive.");
            _cmdBuffers[_activeHandle].drawIndexed(indexCount, instanceCount, firstIndex, vertexOffset, firstInstance);
        }


        /// Binds the vertex buffer to the given command buffer.
        inline void bindVertexBuffer(const size_t bindingIndex, const VulkanBuffer& buffer) const
        {
            TEMPEST_ASSERT(_activeHandle != InvalidCmdBufferHandle,
                           "[CmdBuffer]: No active handle. Set one using setActive.");
            _cmdBuffers[_activeHandle].bindVertexBuffers(static_cast<uint32_t>(bindingIndex), buffer.buffer(), { 0 });
        }


        /// @brief Bind the index buffer to the given command buffer.
        inline void bindIndexBuffer(const size_t offset, vk::IndexType indexType, const VulkanBuffer& buffer) const
        {
            TEMPEST_ASSERT(_activeHandle != InvalidCmdBufferHandle,
                           "[CmdBuffer]: No active handle. Set one using setActive.");
            _cmdBuffers[_activeHandle].bindIndexBuffer(buffer.buffer(), offset, indexType);
        }


        /// @brief vkCmdBeginRendering wrapper.
        inline void beginRendering(const vk::RenderingInfo& info) const noexcept
        {
            TEMPEST_ASSERT(_activeHandle != InvalidCmdBufferHandle,
                           "[CmdBuffer]: No active handle. Set one using setActive.");
            _cmdBuffers[_activeHandle].beginRendering(info);
        }

        /// @brief vkCmdEndRendering wrapper.
        inline void endRendering() const noexcept
        {
            TEMPEST_ASSERT(_activeHandle != InvalidCmdBufferHandle,
                           "[CmdBuffer]: No active handle. Set one using setActive.");
            _cmdBuffers[_activeHandle].endRendering();
        }


        /// @brief Set the viewport on the active command buffer.
        inline void setViewport(const uint32_t firstViewport,
                                const vk::ArrayProxy<const vk::Viewport> viewports) const noexcept
        {
            TEMPEST_ASSERT(_activeHandle != InvalidCmdBufferHandle,
                           "[CmdBuffer]: No active handle. Set one using setActive.");
            _cmdBuffers[_activeHandle].setViewport(firstViewport, viewports);
        }


        /// @brief Set the active command buffer's scissor.
        inline void setScissor(const uint32_t firstScissor, const vk::ArrayProxy<vk::Rect2D const>& scissors)
        {
            TEMPEST_ASSERT(_activeHandle != InvalidCmdBufferHandle,
                           "[CmdBuffer]: No active handle. Set one using setActive.");
            _cmdBuffers[_activeHandle].setScissor(firstScissor, scissors);
        }


        /// +---------------------------------------------------------------------------------------+
        /// |                                 GETTERS                                               |
        /// +---------------------------------------------------------------------------------------+

        // TODO: Update to standard vk type rather than raii
        [[nodiscard]] const vk::raii::CommandBuffer& baseCmdBuffer() const noexcept
        {
            TEMPEST_ASSERT(_activeHandle != InvalidCmdBufferHandle,
                           "[CmdBuffer]: No active handle. Set one using setActive.");
            TEMPEST_ASSERT(_states[_activeHandle] != CommandBufferState::FREED,
                           "Trying to acquired a freed command buffer.");
            return _cmdBuffers[_activeHandle];
        }

        [[nodiscard]] vk::CommandBuffer get(const CmdBufferHandle handle) const noexcept { return _cmdBuffers[handle]; }

        [[nodiscard]] size_t cmdBufferCount() const noexcept { return _cmdBuffers.size(); }


    private:
        vk::raii::CommandPool _cmdPool{ nullptr };
        Vector<vk::raii::CommandBuffer> _cmdBuffers{ makeVector<vk::raii::CommandBuffer>() };
        Vector<CmdBufferHandle> _freeHandles{ makeVector<CmdBufferHandle>() };
        Vector<CommandBufferState> _states{ makeVector<CommandBufferState>() };
        CmdBufferHandle _activeHandle{};
    };
} // namespace tempest::renderer