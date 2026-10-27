#pragma once
/**
 * @file VulkanFence.h
 * @author Alan Abraham P Kochumon
 * @date Created on: October 23, 2026
 *
 * @brief Vulkan Fence abstraction.
 *
 * @copyright Copyright (c) 2026 Alan Abraham P Kochumon
 */

#include "VulkanTypes.inl"

namespace tempest::renderer
{
    class VulkanFence
    {
    public:
        constexpr VulkanFence() noexcept                           = default;
        constexpr VulkanFence(const VulkanFence& other)            = delete;
        constexpr VulkanFence& operator=(const VulkanFence& other) = delete;

        constexpr VulkanFence(VulkanFence&& other) noexcept
            : _isSignaled(other._isSignaled), _fence(std::move(other._fence))
        {}
        constexpr VulkanFence& operator=(VulkanFence&& other) noexcept
        {
            if (this == &other)
                return *this;
            _isSignaled = other._isSignaled;
            _fence      = std::move(other._fence);
            return *this;
        }


        /// Create a @ref VulkanFence.
        /// @param context The vulkan renderer context.
        /// @param createSignaled A bool indicating whether a fence should start its lifetime as signaled.
        void create(const VulkanContext& context, bool createSignaled = false);

        void destroy(const VulkanContext& context) noexcept;

        bool wait(const VulkanContext& context, size_t timeout) noexcept;

        void reset(const VulkanContext& context) noexcept;

        inline vk::raii::Fence& getBaseFence() noexcept { return _fence; }

    private:
        bool _isSignaled{};
        vk::raii::Fence _fence{ nullptr };
    };

} // namespace tempest::renderer