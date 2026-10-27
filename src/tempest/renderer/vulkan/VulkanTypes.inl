#pragma once
/**
 * @file VulkanTypes.inl
 * @author Alan Abraham P Kochumon
 * @date Created on: October 22, 2026
 *
 * @brief Vulkan Structures and Types.
 *
 * @copyright Copyright (c) 2026 Alan Abraham P Kochumon
 */


// Disables Vulkan Constructor generation to support C++20 designated initializers
#define VULKAN_HPP_NO_CONSTRUCTORS
#include "containers/Vector.h"

#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_raii.hpp>


namespace tempest::renderer
{
    //--------------------------------
    //         CONFIGURATION
    //--------------------------------

    struct PhysicalDeviceRequirements
    {
        bool requireGraphicsSupport;
        bool requirePresentSupport;
        bool requireComputeSupport;
        bool requireTransferSupport;

        Vector<const char*> deviceExtensions;

        bool requireDiscreteGPU;

        bool samplerAnisotropy;
        bool dynamicRendering;
        bool shaderDrawParameters;
        bool synchronization2;
        bool extendedDynamicState;
        bool sampleRateShading;
    };


    struct VulkanImageCreateInfo
    {
        vk::ImageType imageType;
        uint32_t width;
        uint32_t height;
        vk::Format format;
        vk::ImageUsageFlags usage;
        vk::MemoryPropertyFlags properties;
        vk::ImageAspectFlags aspectFlags;

        vk::ImageTiling tiling             = vk::ImageTiling::eOptimal;
        bool createView                    = true;
        uint32_t mipLevels                 = 1;
        vk::SampleCountFlagBits numSamples = vk::SampleCountFlagBits::e1;
    };

    struct VulkanLayoutTransitionConfig
    {
        vk::AccessFlags2 srcAccessMask;
        vk::PipelineStageFlags2 srcStageMask;
        vk::AccessFlags2 dstAccessMask;
        vk::PipelineStageFlags2 dstStageMask;

        vk::ImageLayout oldLayout;
        vk::ImageLayout newLayout;
        vk::ImageAspectFlagBits aspectFlag;
        uint32_t mipLevels{ 1 };
        uint32_t layerCount{ 1 };
    };




    //----------------------------------------------------------------
    //         CONTEXTS AND VULKAN STRUCTURE ABSTRACTIONS
    //----------------------------------------------------------------

    /// Type aliases
    using CommandBufferHandle = uint32_t;

    // Forward declarations
    class VulkanFence;



    static constexpr auto MIN_VULKAN_VERSION = VK_API_VERSION_1_3;

    static constexpr auto MAX_FRAMES_IN_FLIGHT_DEF = 2;

    static constexpr uint32_t INVALID_VULKAN_QUEUE_FAMILY_INDEX = ~0;

    struct VulkanQueueFamilyInfo
    {
        uint32_t graphicsFamilyIndex{ INVALID_VULKAN_QUEUE_FAMILY_INDEX };
        uint32_t presentFamilyIndex{ INVALID_VULKAN_QUEUE_FAMILY_INDEX };
        uint32_t computeFamilyIndex{ INVALID_VULKAN_QUEUE_FAMILY_INDEX };
        uint32_t transferFamilyIndex{ INVALID_VULKAN_QUEUE_FAMILY_INDEX };
    };


    struct VulkanSwapChainSupportInfo
    {

        vk::SurfaceCapabilitiesKHR capabilities; ///< Features and capabilities of surface.
        Vector<vk::SurfaceFormatKHR> formats =
            tempest::makeVector<vk::SurfaceFormatKHR>(); ///< Image formats used in rendering.
        Vector<vk::PresentModeKHR> presentModes =
            makeVector<vk::PresentModeKHR>(); ///< Supported present mode(Mailbox, Fifo, Immediate etc.)
    };

    enum class CommandBufferState : uint8_t
    {
        READY,           ///< Command buffer is ready for recording.
        RECORDING,       ///< Command buffer is being recorded.
        RECORDING_ENDED, ///< Command buffer recording has ended.
        SUBMITTED,       ///< Command buffer has been submitted.
        FREED,           ///< Command buffer was freed.
        UNALLOCATED,     ///< Initial command buffer state
    };

} // namespace tempest::renderer