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


#include "VulkanImage.h"
#include "containers/Vector.h"
#include "renderer/vulkan/VulkanTypes.inl"

namespace tempest::renderer
{
    class RenderDevice;
    class VulkanSwapChain
    {
    public:
        constexpr VulkanSwapChain() noexcept
            : _images(makeVector<vk::Image>()),
              _imageViews(makeVector<vk::raii::ImageView>()),
              _depthImages(makeVector<VulkanImage>())
        {}
        void create(VulkanDevice& device, const vk::raii::SurfaceKHR& surface, uint32_t width,
                    uint32_t height) noexcept;
        void recreate(VulkanDevice& device, const vk::raii::SurfaceKHR& surface, uint32_t width,
                      uint32_t height) noexcept;
        void destroy() noexcept;
        uint32_t acquireNextImageIndex(VulkanDevice& device, const vk::raii::SurfaceKHR& surface, size_t timeout,
                                       vk::Semaphore imageAvailableSemaphore, vk::Fence fence);
        void presentSwapChain(VulkanDevice& device, const vk::raii::SurfaceKHR& surface, vk::raii::Queue graphicsQueue,
                              const vk::raii::Queue& presentQueue, vk::Semaphore renderCompleteSemaphore,
                              uint32_t imageIndex);
        [[nodiscard]] uint32_t getImageCount() const noexcept { return _images.size(); }
        [[nodiscard]] uint8_t getMaxFramesInFlight() const noexcept { return _maxFramesInFlight; }
        [[nodiscard]] Vector<vk::raii::ImageView>& getImageViews() noexcept { return _imageViews; }
        [[nodiscard]] Vector<vk::Image>& getImages() noexcept { return _images; }


    private:
        void _createVulkanSwapChain(VulkanDevice& device, const vk::raii::SurfaceKHR& surface, uint32_t width,
                                    uint32_t height, const vk::SwapchainKHR* oldSwapChain = nullptr) noexcept;
        void _cleanupSwapChain() noexcept;
        [[nodiscard]] vk::SurfaceFormatKHR _chooseSurfaceFormat(const VulkanDevice& device,
                                                                const vk::raii::SurfaceKHR& surface) const noexcept;
        [[nodiscard]] vk::PresentModeKHR _choosePresentationMode(const VulkanDevice& device) const noexcept;
        [[nodiscard]] vk::Extent2D _chooseSwapChainExtent(
            const vk::SurfaceCapabilitiesKHR& capabilities) const noexcept;
        [[nodiscard]] uint32_t _chooseMinImageCount(const vk::SurfaceCapabilitiesKHR& capabilities) const noexcept;
        [[nodiscard]] vk::raii::ImageView createImageView(const VulkanDevice& device, const vk::Image& image,
                                                          vk::ImageAspectFlags aspectFlags,
                                                          uint32_t mipLevel) const noexcept;

    private:
        uint8_t _maxFramesInFlight{};
        Vector<vk::Image> _images;
        Vector<vk::raii::ImageView> _imageViews;
        Vector<VulkanImage> _depthImages;
        vk::raii::SwapchainKHR _swapChainInstance{ nullptr };
        vk::Extent2D _extent{};
        vk::SurfaceFormatKHR _format;
        uint32_t _imageIndex{}, _currentFrame{};
        bool _recreatingSwapChain{};
    };
} // namespace tempest::renderer