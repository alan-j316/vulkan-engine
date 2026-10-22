/**
 * @file VulkanDevice.cpp
 * @author Alan Abraham P Kochumon
 * @date Created on: October 22, 2026
 *
 * @brief Implementation of member function declared in VulkanDevice.h
 *
 * @copyright Copyright (c) 2026 Alan Abraham P Kochumon
 */


#include "VulkanDevice.h"

#include "containers/Vector.h"
#include "core/Logger.h"

#include <map>

namespace tempest::renderer
{
    static inline bool pickPhysicalDevice(const VulkanContext& context,
                                          const PhysicalDeviceRequirements& requirements) noexcept;

    static inline bool physicalDeviceMeetsRequirements(const vk::raii::PhysicalDevice& device,
                                                       const vk::raii::SurfaceKHR& surface,
                                                       const PhysicalDeviceRequirements& requirements,
                                                       VulkanQueueFamilyInfo& queueFamilyInfo,
                                                       VulkanSwapChainSupportInfo& swapChainSupport);
    //
    // bool createLogicalDevice(const VulkanContext& context) noexcept
    // {
    //     /// Request a device with graphics family queue
    //     /// and vulkan 1.1 shaderDrawparams, dynamic rendering and extended dynamic state
    //     /// Enable swap chain extension
    //     std::vector<std::pair<vk::QueueFlagBits, RenderQueueType>> queueFlags;
    //     if (config.enableGraphicsQueue)
    //         queueFlags.push_back(std::make_pair(vk::QueueFlagBits::eGraphics, RenderQueueType::GRAPHICS));
    //     if (config.enableSeparateTransferQueue)
    //         queueFlags.push_back(std::make_pair(vk::QueueFlagBits::eTransfer, RenderQueueType::TRANSFER));
    //     if (config.enableComputeQueue)
    //         queueFlags.push_back(std::make_pair(vk::QueueFlagBits::eCompute, RenderQueueType::COMPUTE));
    //
    //     _queues.resize(queueFlags.size());
    //     auto queueProperties = _physicalDevice.getQueueFamilyProperties();
    //     std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos(queueFlags.size());
    //
    //     // Loop through each as per our config and create a queue
    //     for (size_t i = 0; i < queueFlags.size(); ++i)
    //     {
    //         // Iterate through each queue and find the first one that supports both graphics and presentation
    //         for (uint32_t qFamilyIndex = 0; qFamilyIndex < queueProperties.size(); ++qFamilyIndex)
    //         {
    //             if (queueProperties[qFamilyIndex].queueFlags & queueFlags[i].first &&
    //                 _physicalDevice.getSurfaceSupportKHR(qFamilyIndex, *surface.getBaseSurface()))
    //             {
    //                 _queues[i].familyIndex = qFamilyIndex;
    //                 break;
    //             }
    //         }
    //         if (_queues[i].familyIndex == INVALID_QUEUE_INDEX)
    //             return false; // "Couldn't find a queue supporting both graphics and presentation"
    //         _queues[i].type     = queueFlags[i].second;
    //         queueCreateInfos[i] = { .queueFamilyIndex = _queues[i].familyIndex,
    //                                 .queueCount       = 1,
    //                                 .pQueuePriorities = &_queues[i].priority };
    //     }
    //
    //     /// TODO: Abstract request chain to enable user config.
    //     /// Vulkan Feature request chain
    //     /// Combine all the features required into a single struct without using pNext
    //     vk::StructureChain<vk::PhysicalDeviceFeatures2, vk::PhysicalDeviceVulkan11Features,
    //                        vk::PhysicalDeviceVulkan13Features, vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT>
    //         featureChain = {
    //             { .features = { .sampleRateShading = true, .samplerAnisotropy = true } }, // Physical Device Features
    //             { .shaderDrawParameters = true },                                         // Vulkan 1.1 features
    //             { .synchronization2 = true, .dynamicRendering = true },                   // Vulkan 1.3 features
    //             { .extendedDynamicState = true }                                          // Dynamic state
    //         };
    //     const auto ext = _physicalDevice.enumerateDeviceExtensionProperties();
    //     vk::DeviceCreateInfo deviceCreateInfo{ .pNext                =
    //     &featureChain.get<vk::PhysicalDeviceFeatures2>(),
    //                                            .queueCreateInfoCount =
    //                                            static_cast<uint32_t>(queueCreateInfos.size()), .pQueueCreateInfos =
    //                                            queueCreateInfos.data(), .enabledExtensionCount =
    //                                                static_cast<uint32_t>(requiredExtensions.size()),
    //                                            .ppEnabledExtensionNames = requiredExtensions.data() };
    //
    //     _device = vk::raii::Device(_physicalDevice, deviceCreateInfo);
    //
    //
    //     if (_device == nullptr)
    //         return false;
    //
    //     // TODO: Add support multiple queues
    //     for (auto& queue : _queues)
    //     {
    //         queue.queue = vk::raii::Queue(_device, queue.familyIndex, 0);
    //         if (queue.queue == nullptr)
    //         {
    //             log::error("Couldn't create a render queue! Please check for graphics support.");
    //             return false;
    //         }
    //     }
    //     return true;
    // }
    //
    //

    // uint32_t VulkanDevice::findMemoryType(const uint32_t typeFilter,
    //                                       const vk::MemoryPropertyFlags properties) const noexcept
    // {
    //     const vk::PhysicalDeviceMemoryProperties memProperties = _physicalDevice.getMemoryProperties();
    //
    //     for (uint32_t i = 0; i < memProperties.memoryTypeCount; ++i)
    //     {
    //         if (typeFilter & (1 << i) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
    //         {
    //             return i;
    //         }
    //     }
    //
    //     log::error("Failed to find a suitable memory type.");
    //     return std::numeric_limits<uint32_t>::max();
    // }


    // vk::SampleCountFlagBits VulkanDevice::getMaximumSupportSamples() const noexcept
    // {
    //     const vk::PhysicalDeviceProperties properties = _physicalDevice.getProperties();
    //     // Get the sample count for depth and color buffers
    //     const vk::SampleCountFlags count =
    //         properties.limits.framebufferColorSampleCounts & properties.limits.framebufferDepthSampleCounts;
    //     if (count & vk::SampleCountFlagBits::e64)
    //         return vk::SampleCountFlagBits::e64;
    //     if (count & vk::SampleCountFlagBits::e32)
    //         return vk::SampleCountFlagBits::e32;
    //     if (count & vk::SampleCountFlagBits::e16)
    //         return vk::SampleCountFlagBits::e16;
    //     if (count & vk::SampleCountFlagBits::e8)
    //         return vk::SampleCountFlagBits::e8;
    //     if (count & vk::SampleCountFlagBits::e4)
    //         return vk::SampleCountFlagBits::e4;
    //     if (count & vk::SampleCountFlagBits::e2)
    //         return vk::SampleCountFlagBits::e2;
    //     // Return 1 by default
    //     return vk::SampleCountFlagBits::e1;
    // }


    bool VulkanDevice::create(const VulkanContext& context,
                              const PhysicalDeviceRequirements& physicalDeviceRequirements)
    {
        if (!pickPhysicalDevice(context, physicalDeviceRequirements))
        {
            log::error("An error occurred while picking the physical device.");
            return false;
        }


        // +===========================+
        //         QUEUE INDEX
        // +===========================+

        // Make sure no additional queues are created for common queue indices
        bool presentSharesGraphicsQueue  = _queueInfo.graphicsFamilyIndex == _queueInfo.presentFamilyIndex;
        bool transferSharesGraphicsQueue = _queueInfo.graphicsFamilyIndex == _queueInfo.transferFamilyIndex;
        size_t indexCount                = 1;
        if (!presentSharesGraphicsQueue)
        {
            ++indexCount;
        }
        if (!transferSharesGraphicsQueue)
        {
            ++indexCount;
        }

        Vector<uint32_t> indices(indexCount);
        uint32_t index   = 0;
        indices[index++] = _queueInfo.graphicsFamilyIndex;
        if (!presentSharesGraphicsQueue)
        {
            indices[index++] = _queueInfo.presentFamilyIndex;
        }
        if (!transferSharesGraphicsQueue)
        {
            indices[index++] = _queueInfo.transferFamilyIndex;
        }

        Vector<vk::DeviceQueueCreateInfo> queueCreateInfos{};
        for (size_t i = 0; i < indexCount; ++i)
        {
            const auto qIndex = indices[i];
            const auto queueCount = qIndex == _queueInfo.graphicsFamilyIndex ? 2u : 1u;
            vk::DeviceQueueCreateInfo info{ .queueFamilyIndex = qIndex,
                                            .queueCount       = queueCount,
                                            .pQueuePriorities = &_queuePriorities[i] };
            queueCreateInfos.emplace_back(info);
        }



        // +=================================+
        //       LOGICAL DEVICE CREATION
        // +=================================+

        vk::PhysicalDeviceFeatures features;
        features.samplerAnisotropy = physicalDeviceRequirements.samplerAnisotropy;

        vk::StructureChain<vk::PhysicalDeviceFeatures2, vk::PhysicalDeviceVulkan11Features,
                           vk::PhysicalDeviceVulkan13Features, vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT>
            featureChain = {
                { .features = { .sampleRateShading = physicalDeviceRequirements.sampleRateShading,
                                .samplerAnisotropy =
                                    physicalDeviceRequirements.samplerAnisotropy } },        // Physical Device Features
                { .shaderDrawParameters = physicalDeviceRequirements.shaderDrawParameters }, // Vulkan 1.1 features
                { .synchronization2 = physicalDeviceRequirements.synchronization2,
                  .dynamicRendering = physicalDeviceRequirements.dynamicRendering },        // Vulkan 1.3 features
                { .extendedDynamicState = physicalDeviceRequirements.extendedDynamicState } // Dynamic state
            };

        vk::DeviceCreateInfo deviceCreateInfo{
            .pNext                   = &featureChain.get<vk::PhysicalDeviceFeatures2>(),
            .queueCreateInfoCount    = static_cast<uint32_t>(queueCreateInfos.size()),
            .pQueueCreateInfos       = queueCreateInfos.data(),
            .enabledExtensionCount   = static_cast<uint32_t>(physicalDeviceRequirements.deviceExtensions.size()),
            .ppEnabledExtensionNames = physicalDeviceRequirements.deviceExtensions.data()
        };

        _logicalDevice = vk::raii::Device(_physicalDevice, deviceCreateInfo);

        if (_logicalDevice == nullptr)
        {
            log::error("An error occurred while picking the physical device.");
            return false;
        }
        else
        {
            log::info("Logical Device created!");
        }



        // +=================================+
        //          QUEUE RETRIEVAL
        // +=================================+

        _graphicsQueue = vk::raii::Queue(_logicalDevice, _queueInfo.graphicsFamilyIndex, 0);
        _presentQueue  = vk::raii::Queue(_logicalDevice, _queueInfo.presentFamilyIndex, 0);
        _transferQueue = vk::raii::Queue(_logicalDevice, _queueInfo.transferFamilyIndex, 0);


        return true;
    }


    void VulkanDevice::destroy(const VulkanContext& context)
    {

        _graphicsQueue = nullptr;
        _presentQueue  = nullptr;
        _transferQueue = nullptr;

        // Clear present mode and formats
        _swapChainSupport.presentModes.clear();
        _swapChainSupport.formats.clear();
    }


    void VulkanDevice::querySwapChainSupport(const vk::raii::PhysicalDevice& device, const vk::SurfaceKHR& surface,
                                             VulkanSwapChainSupportInfo& info) noexcept
    {
        const auto surfaceFormats = device.getSurfaceFormatsKHR(surface);
        const auto presentModes   = device.getSurfacePresentModesKHR(surface);
        info.capabilities         = device.getSurfaceCapabilitiesKHR(surface);
        // NOTE: We are copying the data from normal vector to pmr vector
        info.formats      = Vector<vk::SurfaceFormatKHR>{ surfaceFormats.begin(), surfaceFormats.end() };
        info.presentModes = Vector<vk::PresentModeKHR>{ presentModes.begin(), presentModes.end() };
    }


    bool VulkanDevice::pickPhysicalDevice(const VulkanContext& context,
                                          const PhysicalDeviceRequirements& requirements) noexcept
    {

        log::info("\n-------- START PHYSICAL DEVICE SELECTION --------");

        /// Properties represent the details about the device like name, vulkan version support etc.
        /// Features represent the feature-set supported by the device like certain shader support
        const auto physicalDevices = vk::raii::PhysicalDevices(context.instance);

        if (physicalDevices.empty())
        {
            log::error("No Graphics card supporting vulkan found!");
            return false;
        }

        std::multimap<uint32_t, vk::raii::PhysicalDevice> gpus;

        for (const auto& pd : physicalDevices)
        {
            const auto properties = pd.getProperties();

            // Check for minimum API version
            if (properties.apiVersion < MIN_VULKAN_VERSION)
                continue;

            if (physicalDeviceMeetsRequirements(pd, context.surface, requirements, _queueInfo, _swapChainSupport))
            {
                // Pick the device and print its details
                _physicalDevice = pd;
                _properties     = properties;
                _features       = _physicalDevice.getFeatures();
                _memory         = _physicalDevice.getMemoryProperties();

                log::info(std::format("Selected Device: {}", std::string(pd.getProperties().deviceName.data())));
                log::info(std::format("GPU Driver Version: {}.{}.{}", VK_VERSION_MAJOR(properties.driverVersion),
                                      VK_VERSION_MINOR(properties.driverVersion),
                                      VK_VERSION_PATCH(properties.driverVersion)));
                log::info(std::format("Vulkan API Version: {}.{}.{}", VK_VERSION_MAJOR(properties.apiVersion),
                                      VK_VERSION_MINOR(properties.apiVersion),
                                      VK_VERSION_PATCH(properties.apiVersion)));

                for (size_t i = 0; i < _memory.memoryHeapCount; ++i)
                {
                    const auto memSize = _memory.memoryHeaps[i].size / (1024.0f * 1024.0f * 1024.0f);
                    if (_memory.memoryHeaps[i].flags & vk::MemoryHeapFlagBits::eDeviceLocal)
                    {
                        log::info(std::format("Local GPU Memory: {:.02} GiB", memSize));
                    }
                    else
                    {
                        log::info(std::format("Shared System Memory: {:.02} GiB", memSize));
                    }
                }
            }
        }
        log::info("--------- END PHYSICAL DEVICE SELECTION ---------\n");

        if (_physicalDevice == nullptr)
        {
            log::error("No physical device selected!");
            return false;
        }

        return true;
    }


    bool physicalDeviceMeetsRequirements(const vk::raii::PhysicalDevice& device, const vk::raii::SurfaceKHR& surface,
                                         const PhysicalDeviceRequirements& requirements,
                                         VulkanQueueFamilyInfo& queueFamilyInfo,
                                         VulkanSwapChainSupportInfo& swapChainSupport)
    {
        const auto deviceProperties = device.getProperties();

        // -------------------------------
        //       DISCRETE GPU SUPPORT
        // -------------------------------
        // Check if the current device is discrete GPU as required by the `requirements`
        if (requirements.requireDiscreteGPU && deviceProperties.deviceType != vk::PhysicalDeviceType::eDiscreteGpu)
        {
            return false;
        }

        // -------------------------------
        //       QUEUE FAMILY SUPPORT
        // -------------------------------
        const auto queueFamilies = device.getQueueFamilyProperties();
        // Keep a transfer score to ensure that we can get a separate transfer queue
        // Kohi: https://www.youtube.com/watch?v=6Kj3O2Ov1RU&list=PLv8Ddw9K0JPg1BEO-RS-0MYs423cvLVtj&index=17
        uint8_t minTransferScore = 0xff;
        for (uint32_t i = 0; i < queueFamilies.size(); ++i)
        {
            uint8_t currentTransferScore = 0;
            const auto& queueFamily      = queueFamilies[i];
            // Graphics Queue
            if (requirements.requireGraphicsSupport && (queueFamily.queueFlags & vk::QueueFlagBits::eGraphics))
            {
                queueFamilyInfo.graphicsFamilyIndex = i;
                ++currentTransferScore;
            }

            // Transfer Queue
            if (requirements.requireTransferSupport && (queueFamily.queueFlags & vk::QueueFlagBits::eTransfer))
            {
                queueFamilyInfo.transferFamilyIndex = i;
                ++currentTransferScore;
            }

            // Compute Queue
            if (requirements.requireComputeSupport && (queueFamily.queueFlags & vk::QueueFlagBits::eCompute))
            {
                // Take the lowest transfer, and thereby increasing the likelihood of getting a separate
                // transfer queue.
                if (currentTransferScore <= minTransferScore)
                {
                    minTransferScore                   = currentTransferScore;
                    queueFamilyInfo.computeFamilyIndex = i;
                }
            }

            // Present Queue
            if (requirements.requirePresentSupport && device.getSurfaceSupportKHR(i, surface))
            {
                queueFamilyInfo.presentFamilyIndex = i;
            }
        }

        // TODO: Refactor
        const bool graphicsReqSatisfied = !requirements.requireGraphicsSupport ||
            (requirements.requireGraphicsSupport &&
             queueFamilyInfo.graphicsFamilyIndex != INVALID_VULKAN_QUEUE_FAMILY_INDEX);
        const bool computeReqSatisfied = !requirements.requireComputeSupport ||
            (requirements.requireComputeSupport &&
             queueFamilyInfo.computeFamilyIndex != INVALID_VULKAN_QUEUE_FAMILY_INDEX);
        const bool transferReqSatisfied = !requirements.requireTransferSupport ||
            (requirements.requireTransferSupport &&
             queueFamilyInfo.transferFamilyIndex != INVALID_VULKAN_QUEUE_FAMILY_INDEX);
        const bool presentReqSatisfied = !requirements.requirePresentSupport ||
            (requirements.requirePresentSupport &&
             queueFamilyInfo.presentFamilyIndex != INVALID_VULKAN_QUEUE_FAMILY_INDEX);
        if (graphicsReqSatisfied && computeReqSatisfied && transferReqSatisfied && presentReqSatisfied)
        {
            log::info("\n-- Device meets requirements --\n-- Selected Queue Indices --");
            log::info(std::format("Graphics Family Index: {}", queueFamilyInfo.graphicsFamilyIndex));
            log::info(std::format("Compute Family Index: {}", queueFamilyInfo.computeFamilyIndex));
            log::info(std::format("Transfer Family Index: {}", queueFamilyInfo.transferFamilyIndex));
            log::info(std::format("Present Family Index: {}\n", queueFamilyInfo.presentFamilyIndex));
        }
        else
        {
            log::warn("Some queue families are not supported by the device");
            return false;
        }


        // -------------------------------
        //        SWAP CHAIN SUPPORT
        // -------------------------------
        VulkanDevice::querySwapChainSupport(device, surface, swapChainSupport);
        if (swapChainSupport.formats.size() < 1 || swapChainSupport.presentModes.size() < 1)
        {
            log::error("Device doesn't meet the requirement for a swapchain(Format and Present Modes)");
            return false;
        }


        // -------------------------------
        //        DEVICE EXTENSIONS
        // -------------------------------
        const auto supportedExtensions = device.enumerateDeviceExtensionProperties();
        bool supportsAllRequiredExtensions =
            std::ranges::all_of(requirements.deviceExtensions, [&supportedExtensions](const auto& requiredExtension) {
                return std::ranges::any_of(supportedExtensions, [requiredExtension](const auto& supportedExtension) {
                    return std::strcmp(supportedExtension.extensionName, requiredExtension);
                });
            });
        if (!supportsAllRequiredExtensions)
        {
            log::error("Device doesn't support one or more required extensions.");
            return false;
        }


        // -------------------------------
        //        DEVICE FEATURES
        // -------------------------------
        const auto deviceFeatures =
            device
                .getFeatures2<vk::PhysicalDeviceFeatures2, vk::PhysicalDeviceVulkan11Features,
                              vk::PhysicalDeviceVulkan13Features, vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT>();
        if (requirements.samplerAnisotropy &&
            !deviceFeatures.get<vk::PhysicalDeviceFeatures2>().features.samplerAnisotropy)
        {
            log::error("Device doesn't support sample anisotropy. Skipping...");
            return false;
        }

        if (requirements.dynamicRendering && !deviceFeatures.get<vk::PhysicalDeviceVulkan13Features>().dynamicRendering)
        {
            log::error("Device doesn't support dynamic rendering. Skipping...");
            return false;
        }

        if (requirements.shaderDrawParameters &&
            !deviceFeatures.get<vk::PhysicalDeviceVulkan11Features>().shaderDrawParameters)
        {
            log::error("Device doesn't support shader draw parameters. Skipping...");
            return false;
        }

        if (requirements.synchronization2 && !deviceFeatures.get<vk::PhysicalDeviceVulkan13Features>().synchronization2)
        {
            log::error("Device doesn't support newer synchronization features(v2). Skipping...");
            return false;
        }

        if (requirements.extendedDynamicState &&
            !deviceFeatures.get<vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT>().extendedDynamicState)
        {
            log::error("Device doesn't support extended dynamic state. Skipping...");
            return false;
        }

        // All requirements satisfied.
        return true;
    }


} // namespace tempest::renderer
