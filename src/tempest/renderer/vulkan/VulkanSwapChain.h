#pragma once
/**
 * @file VulkanSwapchain.h
 * @author Alan Abraham P Kochumon
 * @date Created on: October 22, 2026
 *
 * @brief Swapchain abstraction.
 *
 * @copyright Copyright (c) 2026 Alan Abraham P Kochumon
 */


#include "containers/Vector.h"
#include "renderer/vulkan/VulkanTypes.inl"

namespace tempest::renderer
{
    class RenderDevice;
    class SwapChain
    {
    public:
        void create(VulkanContext& context, uint32_t width, uint32_t height) noexcept;
        void recreate(VulkanContext& context, uint32_t width, uint32_t height) noexcept;
        void destroy(const VulkanContext& context) noexcept;
        uint32_t acquireNextImageIndex(VulkanContext& context, size_t timeout, vk::Semaphore imageAvailableSemaphore,
                                       vk::Fence fence);
        void presentSwapChain(VulkanContext& context, vk::raii::Queue graphicsQueue,
                              const vk::raii::Queue& presentQueue, vk::Semaphore renderCompleteSemaphore,
                              uint32_t imageIndex);


    private:
        void _createVulkanSwapChain(VulkanContext& context, uint32_t width, uint32_t height,
                                    const vk::SwapchainKHR* oldSwapChain = nullptr) noexcept;
        void _cleanupSwapChain() noexcept;
        [[nodiscard]] vk::SurfaceFormatKHR _chooseSurfaceFormat(const VulkanContext& context) const noexcept;
        [[nodiscard]] vk::PresentModeKHR _choosePresentationMode(const VulkanContext& context) const noexcept;
        [[nodiscard]] vk::Extent2D _chooseSwapChainExtent(
            const vk::SurfaceCapabilitiesKHR& capabilities) const noexcept;
        [[nodiscard]] uint32_t _chooseMinImageCount(const vk::SurfaceCapabilitiesKHR& capabilities) const noexcept;
        [[nodiscard]] vk::raii::ImageView createImageView(const VulkanContext& context, const vk::Image& image,
                                                          vk::ImageAspectFlags aspectFlags,
                                                          uint32_t mipLevel) const noexcept;
        //
        // void createImageViews() noexcept;
        // [[nodiscard]] vk::SurfaceCapabilitiesKHR querySurfaceCapabilities() const noexcept;

    private:
        uint8_t _maxFramesInFlight{};
        Vector<vk::Image> _images;
        Vector<vk::raii::ImageView> _imageViews;
        vk::raii::SwapchainKHR _swapChainInstance{ nullptr };
        vk::Extent2D _extent{};
        vk::SurfaceFormatKHR _format;
        uint32_t _imageIndex{}, _currentFrame{};
        bool _recreatingSwapChain{};
    };
} // namespace tempest::renderer