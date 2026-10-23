/**
 * @file VulkanSwapchain.cpp
 * @author Alan Abraham P Kochumon
 * @date Created on: October 22, 2026
 *
 * @brief Implementation of member functions declared in Swapchain.h
 *
 * @copyright Copyright (c) 2026 Alan Abraham P Kochumon
 */

#include "VulkanSwapChain.h"

#include "VulkanDevice.h"
#include "core/Logger.h"
#include "core/MemorySubsystem.h"

namespace tempest::renderer
{
    void VulkanSwapChain::create(VulkanContext& context, const uint32_t width, const uint32_t height) noexcept
    { _createVulkanSwapChain(context, width, height); }

    void VulkanSwapChain::recreate(VulkanContext& context, const uint32_t width, const uint32_t height) noexcept
    { _createVulkanSwapChain(context, width, height, &*_swapChainInstance); }

    void VulkanSwapChain::destroy(const VulkanContext& context) noexcept { _cleanupSwapChain(); }

    uint32_t VulkanSwapChain::acquireNextImageIndex(VulkanContext& context, const size_t timeout,
                                                    const vk::Semaphore imageAvailableSemaphore, const vk::Fence fence)
    {
        const auto [result, index] = _swapChainInstance.acquireNextImage(timeout, imageAvailableSemaphore, nullptr);
        if (result == vk::Result::eErrorOutOfDateKHR)
        {
            recreate(context, _extent.width, _extent.height);
        }
        if (result != vk::Result::eSuccess && result != vk::Result::eSuboptimalKHR)
        {
            log::warn(std::format("An error occurred while acquiring swapchain image."));
        }

        return index;
    }


    void VulkanSwapChain::presentSwapChain(VulkanContext& context, vk::raii::Queue graphicsQueue,
                                           const vk::raii::Queue& presentQueue, vk::Semaphore renderCompleteSemaphore,
                                           uint32_t imageIndex)
    {
        const vk::PresentInfoKHR presentInfo{
            .waitSemaphoreCount = 1,
            .pWaitSemaphores    = &renderCompleteSemaphore,
            .swapchainCount     = 1,
            .pSwapchains        = &*_swapChainInstance,
            .pImageIndices      = &imageIndex,
            .pResults           = nullptr, // Allows you to check whether the swapChain presentation was successful
        };

        if (const auto result = presentQueue.presentKHR(presentInfo); vk::Result::eSuccess != result)
        {
            log::error("There was an error presenting the swap chain");
        }
        else if (vk::Result::eErrorOutOfDateKHR == result || vk::Result::eSuboptimalKHR == result)
        {
            recreate(context, _extent.width, _extent.height);
        }
    }




    // +===============================
    //        PRIVATE METHODS
    // +===============================

    void VulkanSwapChain::_createVulkanSwapChain(VulkanContext& context, uint32_t width, uint32_t height,
                                                 const vk::SwapchainKHR* oldSwapChain) noexcept
    {
        _maxFramesInFlight           = MAX_FRAMES_IN_FLIGHT_DEF;
        _extent                      = { .width = width, .height = height };
        context.swapChainExtent      = _extent;
        const auto capabilities      = context.device->getSwapChainSupport().capabilities;
        _extent                      = _chooseSwapChainExtent(capabilities);
        const auto& vkSurface        = context.surface;
        const uint32_t minImageCount = _chooseMinImageCount(capabilities);

        _format                          = _chooseSurfaceFormat(context);
        const auto availablePresentModes = context.device->getPhysicalDevice().getSurfacePresentModesKHR(vkSurface);

        // If the present and graphics family are not the same then we need to make the sharing mode to
        // concurrent and allow the images to be shared
        const auto& queueInfo      = context.device->getQueueInfo();
        const bool differentFamily = queueInfo.graphicsFamilyIndex != queueInfo.presentFamilyIndex;
        const auto sharingMode     = differentFamily ? vk::SharingMode::eConcurrent : vk::SharingMode::eExclusive;
        const uint32_t queueFamilyIndices[]{ queueInfo.graphicsFamilyIndex, queueInfo.presentFamilyIndex };
        // NOTE: We can vary the queue family index count only and still get the same results
        const uint32_t queueFamilyIndexCount = differentFamily ? 2 : 0;

        const vk::SwapchainCreateInfoKHR swapChainCreateInfo{
            .surface          = vkSurface,
            .minImageCount    = minImageCount,
            .imageFormat      = _format.format,
            .imageColorSpace  = _format.colorSpace,
            .imageExtent      = _extent,
            .imageArrayLayers = 1, // Always 1 unless for stereoscopic 3D
            // The image usage, for intermediate ops use ::eTransferDst
            .imageUsage = vk::ImageUsageFlagBits::eColorAttachment,
            // Ownership must be exclusively transferred to other queue
            // eConcurrent: Shared by multiple queue without exclusive ownership transfer
            .imageSharingMode      = sharingMode,
            .queueFamilyIndexCount = queueFamilyIndexCount,
            .pQueueFamilyIndices   = queueFamilyIndices,
            // SupportTransforms from capabilities. To specify no transform provide currentTransform.
            .preTransform = capabilities.currentTransform,
            // Whether to use alpha channel for blending with other windows, almost always false
            .compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque,
            .presentMode    = _choosePresentationMode(context),
            .clipped        = true, // Clip obscured pixels
            // For swap chain recreation
            .oldSwapchain = oldSwapChain == nullptr ? nullptr : *oldSwapChain

        };

        _swapChainInstance = vk::raii::SwapchainKHR(context.device->getLogicalDevice(), swapChainCreateInfo);
        const auto images  = _swapChainInstance.getImages();
        _images            = { images.begin(), images.end() };

        _imageIndex = 0;


        // +=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=
        //         IMAGE VIEWS
        // +=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=

        _imageViews.reserve(images.size());
        for (const auto& image : images)
        {
            _imageViews.emplace_back(createImageView(context, image, vk::ImageAspectFlagBits::eColor, 1));
        }


        // +=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=
        //         DEPTH RESOURCES
        // +=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=

        if (!context.device->detectDepthFormat())
        {
            context.device->setDepthFormat(vk::Format::eUndefined);
            log::error("There was an error creating the depth format");
        }

        _imageViews.reserve(images.size());
        for (size_t i = 0; i < images.size(); ++i)
        {
            VulkanImageCreateInfo createInfo{ .imageType   = vk::ImageType::e2D,
                                              .width       = _extent.width,
                                              .height      = _extent.height,
                                              .format      = _format.format,
                                              .usage       = vk::ImageUsageFlagBits::eDepthStencilAttachment,
                                              .properties  = vk::MemoryPropertyFlagBits::eDeviceLocal, // TODO: Update
                                              .aspectFlags = vk::ImageAspectFlagBits::eDepth,
                                              .tiling      = vk::ImageTiling::eOptimal,
                                              .createView  = true,
                                              .mipLevels   = 1,
                                              .numSamples  = vk::SampleCountFlagBits::e1 };
            VulkanImage depthImage{};
            depthImage.create(context, createInfo);
            _depthImages.push_back(std::move(depthImage));
        }

        log::info("SwapChain created successfully!");
    }


    void VulkanSwapChain::_cleanupSwapChain() noexcept
    {
        // Rest will be done by vk::raii dtor
        _imageViews.clear();
        _images.clear();
        _depthImages.clear();
        _swapChainInstance = nullptr;
    }


    vk::SurfaceFormatKHR VulkanSwapChain::_chooseSurfaceFormat(const VulkanContext& context) const noexcept
    {
        // Query the surface format
        const auto surfaceFormats = context.device->getPhysicalDevice().getSurfaceFormatsKHR(context.surface);
        assert(surfaceFormats.size() > 0);
        // Find a suitable format with srgb colorspace, and return the first one if not supported
        const auto formatIt = std::ranges::find_if(surfaceFormats, [](const auto& surfaceFormat) {
            return surfaceFormat.format == vk::Format::eB8G8R8A8Srgb &&
                surfaceFormat.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear;
        });
        return formatIt != surfaceFormats.end() ? *formatIt : surfaceFormats[0];
    }


    uint32_t VulkanSwapChain::_chooseMinImageCount(const vk::SurfaceCapabilitiesKHR& capabilities) const noexcept
    {
        // Choose an appropriate image count in the range between minImageCount < n <= maxImageCount/3
        auto minImageCount = std::max(3u, capabilities.minImageCount); // Choose between min and images max
        if (capabilities.maxImageCount > 0 && capabilities.maxImageCount < minImageCount)
        {
            minImageCount = capabilities.maxImageCount;
        }
        return minImageCount;
    }

    vk::Extent2D VulkanSwapChain::_chooseSwapChainExtent(const vk::SurfaceCapabilitiesKHR& capabilities) const noexcept
    {
        // Choose the resolution of the swap chain
        // If the current extend is not the max value for uint32_t(set by some window managers)
        // then use the current extent, otherwise use the SDL buffer's width and height
        // This is necessary since some displays can have a larger buffer like apple's retina display w * dpi
        if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
            return capabilities.currentExtent;

        return vk::Extent2D{
            .width  = std::clamp<uint32_t>(_extent.width, capabilities.minImageExtent.width,
                                           capabilities.maxImageExtent.width),
            .height = std::clamp<uint32_t>(_extent.height, capabilities.minImageExtent.height,
                                           capabilities.maxImageExtent.height),
        };
    }

    vk::PresentModeKHR VulkanSwapChain::_choosePresentationMode(const VulkanContext& context) const noexcept
    {
        const auto supportedPresentModes = context.device->getSwapChainSupport().presentModes;
        if (std::ranges::find(supportedPresentModes, vk::PresentModeKHR::eMailbox) != supportedPresentModes.end())
        {
            return vk::PresentModeKHR::eMailbox;
        }
        return vk::PresentModeKHR::eFifo; // V-Sync fallback
    }


    vk::raii::ImageView VulkanSwapChain::createImageView(const VulkanContext& context, const vk::Image& image,
                                                         const vk::ImageAspectFlags aspectFlags,
                                                         const uint32_t mipLevel) const noexcept
    {
        const vk::ImageViewCreateInfo viewInfo{ .image            = image,
                                                .viewType         = vk::ImageViewType::e2D,
                                                .format           = _format.format,
                                                .subresourceRange = { .aspectMask     = aspectFlags,
                                                                      .baseMipLevel   = 0,
                                                                      .levelCount     = mipLevel,
                                                                      .baseArrayLayer = 0,
                                                                      .layerCount     = 1 } };

        return vk::raii::ImageView(context.device->getLogicalDevice(), viewInfo);
    }

} // namespace tempest::renderer