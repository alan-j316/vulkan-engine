/**
 * @file VulkanBackend.cpp
 * @author Alan Abraham P Kochumon
 * @date Created on: October 22, 2026
 *
 * @brief Vulkan Backend member function implementation.
 *
 * @copyright Copyright (c) 2026 Alan Abraham P Kochumon
 */


#include "VulkanBackend.h"

#include "VulkanCommandPool.h"
#include "VulkanFence.h"
#include "VulkanTypes.inl"
#include "core/Asserts.h"
#include "core/Logger.h"
#include "core/MemorySubsystem.h"
#include "platform/Platform.h"
#include "renderer/vulkan/VulkanDevice.h"
#include "renderer/vulkan/VulkanFence.h"
#include "renderer/vulkan/VulkanPlatform.h"
#include "renderer/vulkan/VulkanSwapChain.h"


namespace tempest::renderer
{
    static VulkanContext context;
    // TODO: Move to config
    static constexpr auto ENGINE_NAME      = "Tempest";
    static constexpr size_t APP_VERSION    = 1;
    static constexpr size_t ENGINE_VERSION = 1;
#ifndef NDEBUG
    static constexpr bool ENABLE_VALIDATION_LAYERS = true;
    static std::vector<const char*> VALIDATION_LAYERS{ "VK_LAYER_KHRONOS_validation" };
#else
    static constexpr bool ENABLE_VALIDATION_LAYERS = false;
    static std::vector<const char*> VALIDATION_LAYERS{};
#endif

    /// Debug Callback
    static vk::Bool32 debugCallback(vk::DebugUtilsMessageSeverityFlagBitsEXT severity,
                                    vk::DebugUtilsMessageTypeFlagsEXT type,
                                    const vk::DebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData);

    static void setupDebugMessenger() noexcept;


    bool VulkanBackend::init(platform::PlatformState* state, std::string_view applicationName) noexcept
    {
        _state = state;
        const vk::ApplicationInfo applicationInfo{ .pApplicationName   = applicationName.data(),
                                                   .applicationVersion = VK_MAKE_VERSION(APP_VERSION, 0, 0),
                                                   .pEngineName        = ENGINE_NAME,
                                                   .engineVersion      = VK_MAKE_VERSION(ENGINE_VERSION, 0, 0),
                                                   .apiVersion         = static_cast<uint32_t>(MIN_VULKAN_VERSION) };

        //------------
        // EXTENSIONS
        //------------
        const auto requiredExtensions  = getRequiredExtensions();
        const auto extensionProperties = context.baseContext.enumerateInstanceExtensionProperties();
        log::info("\nRequired Vulkan Extensions");
        for (const auto ext : requiredExtensions)
        {
            log::info(ext);
        }
        log::info("\n");

        // Check if the extension we require are support by vulkan
        const auto unsupportedPropertiesIt =
            std::ranges::find_if(requiredExtensions, [&extensionProperties](const auto& requiredExtension) {
                return std::ranges::none_of(extensionProperties, [requiredExtension](const auto& extensionProperty) {
                    return strcmp(extensionProperty.extensionName, requiredExtension) == 0;
                });
            });
        if (unsupportedPropertiesIt != requiredExtensions.end())
        {
            log::error("Required extension(s) not supported!");
            return false;
        }



        //--------
        // LAYERS
        //--------
        Vector<const char*> requiredLayers = makeVector<const char*>();
        if constexpr (ENABLE_VALIDATION_LAYERS)
        {
            requiredLayers.assign(VALIDATION_LAYERS.begin(), VALIDATION_LAYERS.end());
        }
        // Enumerate through each layer and determine if the layers we need are supported
        const auto supportedLayers = context.baseContext.enumerateInstanceLayerProperties();
        const auto unsupportedLayers =
            std::ranges::find_if(requiredLayers, [&supportedLayers](const auto& requiredLayer) {
                return std::ranges::none_of(supportedLayers, [requiredLayer](const auto& layerProperty) {
                    return strcmp(layerProperty.layerName, requiredLayer) == 0;
                });
            });

        if (unsupportedLayers != requiredLayers.end())
        {
            log::error("Required layer(s) not supported!");
            return false;
        }


        //-------------------
        // INSTANCE CREATION
        //-------------------
        const vk::InstanceCreateInfo instanceCreateInfo{
            .pApplicationInfo        = &applicationInfo,
            .enabledLayerCount       = static_cast<uint32_t>(requiredLayers.size()),
            .ppEnabledLayerNames     = requiredLayers.data(),
            .enabledExtensionCount   = static_cast<uint32_t>(requiredExtensions.size()),
            .ppEnabledExtensionNames = requiredExtensions.data(),
        };

        context.instance = vk::raii::Instance(context.baseContext, instanceCreateInfo, context.allocator);
        setupDebugMessenger();


        // -------------------
        //  SURFACE CREATION
        // -------------------
        if (!platform::createVulkanSurface(*_state, context))
        {
            log::error("There was an error creating vulkan surface");
            return false;
        }


        // -------------------
        //  DEVICE CREATION
        // -------------------
        // TODO: Move device to class member
        const PhysicalDeviceRequirements physicalDeviceReq{ .requireGraphicsSupport = true,
                                                            .requirePresentSupport  = true,
                                                            .requireComputeSupport  = true,
                                                            .requireTransferSupport = true,
                                                            .deviceExtensions =
                                                                Vector{ VK_KHR_SWAPCHAIN_EXTENSION_NAME },
                                                            .requireDiscreteGPU   = true,
                                                            .samplerAnisotropy    = true,
                                                            .dynamicRendering     = true,
                                                            .shaderDrawParameters = true,
                                                            .synchronization2     = true,
                                                            .extendedDynamicState = true,
                                                            .sampleRateShading    = true };

        // TODO: Look for a way to create a stack based device
        const auto rawDevMem = memory::alloc(sizeof(VulkanDevice), memory::MemoryTag::MEMORY_TAG_RENDERER);
        context.device       = new (rawDevMem) VulkanDevice();

        if (!context.device->create(context, physicalDeviceReq))
        {
            log::error("There was an error creating the vulkan device");
            return false;
        }

        const auto rawSwapChainMem = memory::alloc(sizeof(VulkanSwapChain), memory::MemoryTag::MEMORY_TAG_RENDERER);
        context.swapChain          = new (rawSwapChainMem) VulkanSwapChain();

        // ---------------------
        //  SWAP CHAIN CREATION
        // ---------------------
        // TODO: Update width and height
        context.swapChain->create(context, 1280, 720);


        // -------------------------
        //  COMMAND POOL AND BUFFERS
        // -------------------------
        createGraphicsCommandPoolAndAllocateBuffers();


        // ------------------------
        //  SYNCHRONIZATION OBJECTS
        // ------------------------
        context.inFlightFences = makeVector<VulkanFence>();
        context.inFlightFences.resize(context.swapChain->getMaxFramesInFlight());

        for (size_t i = 0; i < context.swapChain->getMaxFramesInFlight(); ++i)
        {
            vk::SemaphoreCreateInfo semaphoreCreateInfo{};
            context.imageAvailableSemaphores.emplace_back(context.device->getLogicalDevice(), semaphoreCreateInfo,
                                                          context.allocator);
            context.queueCompleteSemaphore.emplace_back(context.device->getLogicalDevice(), semaphoreCreateInfo,
                                                        context.allocator);

            VulkanFence fence{};
            fence.create(context, true);
            context.inFlightFences[i] = std::move(fence);
        }

        log::info("Vulkan Renderer Initialized");
        return context.instance != nullptr;
    }


    void VulkanBackend::shutdown() noexcept
    {
        // Wait for idle before shutting down sync primitives
        context.device->getLogicalDevice().waitIdle();

        // Destroy sync primitives
        context.imageAvailableSemaphores.clear();
        context.queueCompleteSemaphore.clear();
        context.inFlightFences.clear();
        context.swapChain->destroy(context);

        context.graphicsCommandPool->destroy();
        context.device->destroy(context);
        context.surface  = nullptr;
        context.instance = nullptr;
        log::info("Vulkan Backend successfully shutdown!");
        // NOTE: RAII will do rest of the cleanup
    }

    void VulkanBackend::onResize(const uint32_t width, const uint32_t height) noexcept
    { context.swapChain->recreate(context, width, height); }

    //
    // void VulkanBackend::recordCommandBuffer(const uint32_t imageIndex) const noexcept
    // {
    //     auto commandBufferHandles = context.graphicsCommandBuffers;
    //
    //     context.graphicsCommandPool->startRecording(commandBufferHandles[context.frameIndex], false, false);
    //
    //     // Transition swap chain image to ImageLayout::eColorAttachmentOptimal
    //     transitionImageLayout(swapChainImages[imageIndex], vk::ImageLayout::eUndefined,
    //                           vk::ImageLayout::eColorAttachmentOptimal, {}, // no need to wait for previous operation
    //                           vk::AccessFlagBits2::eColorAttachmentWrite,
    //                           vk::PipelineStageFlagBits2::eColorAttachmentOutput,
    //                           vk::PipelineStageFlagBits2::eColorAttachmentOutput, vk::ImageAspectFlagBits::eColor);
    //     // Transition depth image to depth attachment optimal layout
    //     // We need a single barrier for depth attachment writes(we don't care after rendering).
    //     transitionImageLayout(
    //         *depthImage, vk::ImageLayout::eUndefined, vk::ImageLayout::eDepthAttachmentOptimal,
    //         vk::AccessFlagBits2::eDepthStencilAttachmentWrite, vk::AccessFlagBits2::eDepthStencilAttachmentWrite,
    //         vk::PipelineStageFlagBits2::eEarlyFragmentTests | vk::PipelineStageFlagBits2::eLateFragmentTests,
    //         vk::PipelineStageFlagBits2::eEarlyFragmentTests | vk::PipelineStageFlagBits2::eLateFragmentTests,
    //         vk::ImageAspectFlagBits::eDepth);
    //
    //     // Transition the multisampled image
    //     // transitionImageLayout(*colorImage, vk::ImageLayout::eUndefined, vk::ImageLayout::eColorAttachmentOptimal,
    //     //                       vk::AccessFlagBits2::eColorAttachmentWrite,
    //     vk::AccessFlagBits2::eColorAttachmentWrite,
    //     //                       vk::PipelineStageFlagBits2::eColorAttachmentOutput,
    //     //                       vk::PipelineStageFlagBits2::eColorAttachmentOutput,
    //     vk::ImageAspectFlagBits::eColor);
    //
    //     constexpr vk::ClearColorValue clearColor{ 0.0f, 0.0f, 0.0f, 1.0f };
    //     // constexpr vk::ClearDepthStencilValue clearDepth{ .depth = 1.0f, .stencil = 0 };
    //
    //     // Dynamic rendering doesn't require a RenderPass but we need to specify the attachment info
    //     vk::RenderingAttachmentInfo attachmentInfo{
    //         .imageView          = colorImageView,
    //         .imageLayout        = vk::ImageLayout::eColorAttachmentOptimal,
    //         .resolveMode        = vk::ResolveModeFlagBits::eAverage,
    //         .resolveImageView   = context.swapChain->getImageViews[imageIndex],
    //         .resolveImageLayout = vk::ImageLayout::eColorAttachmentOptimal,
    //         .loadOp             = vk::AttachmentLoadOp::eClear,  // What to do to the attachment before rendering
    //         .storeOp            = vk::AttachmentStoreOp::eStore, // What to do to the attachment after rendering
    //         .clearValue         = clearColor
    //     };
    //     //
    //     // vk::RenderingAttachmentInfo depthAttachmentInfo{ .imageView   = depthImageView,
    //     //                                                  .imageLayout = vk::ImageLayout::eDepthAttachmentOptimal,
    //     //                                                  .loadOp      = vk::AttachmentLoadOp::eClear,
    //     //                                                  .storeOp     = vk::AttachmentStoreOp::eDontCare,
    //     //                                                  .clearValue  = clearDepth };
    //
    //     // Create the rendering info
    //     const vk::RenderingInfo renderingInfo{ .renderArea           = { .offset = { .x = 0, .y = 0 },
    //                                                                      .extent = context.swapChainExtent },
    //                                            .layerCount           = 1,
    //                                            .colorAttachmentCount = 1,
    //                                            .pColorAttachments    = &attachmentInfo,
    //                                            .pDepthAttachment     = nullptr };
    //     // .pDepthAttachment     = &depthAttachmentInfo };
    //
    //     // Begin Rendering
    //     // commandBuffers[frameIndex].beginRendering(renderingInfo);
    //     // commandBuffers[frameIndex].bindPipeline(vk::PipelineBindPoint::eGraphics, *graphicsPipeline);
    //     // // Bind the vertex buffer
    //     // commandBuffers[frameIndex].bindVertexBuffers(0, *vertexBuffer, { 0 });
    //     // commandBuffers[frameIndex].bindIndexBuffer(*indexBuffer, 0,
    //     //                                            vk::IndexTypeValue<decltype(indices)::value_type>::value);
    //     // Set dynamic states
    //     // NOTE: The negative height is due to the fact the glm has an inverted y axis in the projection matrix.
    //     commandBuffers[frameIndex].setViewport(0,
    //                                            vk::Viewport(0.0f, static_cast<float>(swapChainExtent.height),
    //                                                         static_cast<float>(swapChainExtent.width),
    //                                                         -static_cast<float>(swapChainExtent.height),
    //                                                         0.0f, 1.0f));
    //     commandBuffers[frameIndex].setScissor(0, vk::Rect2D(vk::Offset2D(0, 0), swapChainExtent));
    //
    //     // Bind the descriptor set for uniforms
    //     // commandBuffers[frameIndex].bindDescriptorSets(vk::PipelineBindPoint::eGraphics, pipelineLayout, 0,
    //     //                                               *descriptorSets[frameIndex], nullptr);
    //     // Draw
    //     // commandBuffers[frameIndex].drawIndexed(static_cast<uint32_t>(indices.size()), 1, 0, 0, 0);
    //     // End rendering
    //     commandBuffers[frameIndex].endRendering();
    //
    //     // Transition image layout for presentation
    //     transitionImageLayout(swapChainImages[imageIndex], vk::ImageLayout::eColorAttachmentOptimal,
    //                           vk::ImageLayout::ePresentSrcKHR, vk::AccessFlagBits2::eColorAttachmentWrite, {},
    //                           vk::PipelineStageFlagBits2::eColorAttachmentOutput,
    //                           vk::PipelineStageFlagBits2::eBottomOfPipe, vk::ImageAspectFlagBits::eColor);
    //     commandBuffers[frameIndex].end();
    // }

    bool VulkanBackend::beginFrame(float deltaTime) noexcept
    {
        // TODO: Handle swapchain recreation
        auto& device = context.device->getLogicalDevice();
        // Wait for previous frame to be finished
        if (!context.inFlightFences[context.frameIndex].wait(context, UINT64_MAX))
        {
            log::error("An error occurred while waiting for previous frame to finish");
            return false;
        }
        // Reset the fence
        context.inFlightFences[context.frameIndex].reset(context);
        device.waitIdle(); // TODO: Replace with fence sync
        // Acquire the next image index
        context.swapChainImageIndex = context.swapChain->acquireNextImageIndex(
            context, UINT64_MAX, context.imageAvailableSemaphores[context.frameIndex], nullptr);



        // ------------------------------
        //     COMMAND BUFFER RECORDING
        // ------------------------------
        // Get the command buffer handle, reset and begin recording.
        const auto bufferHandle = context.graphicsCommandBuffers[context.frameIndex];
        context.graphicsCommandPool->reset(bufferHandle);
        // NOTE: We are resetting the command buffer and re-recording it every frame.
        context.graphicsCommandPool->startRecording(bufferHandle, true, false);

        // log::warn("Unsupported layout transition; using a suboptimal fallback transition.");
        // TODO: Update transition flags
        constexpr VulkanLayoutTransitionConfig transitionConfig{
            .srcAccessMask = vk::AccessFlagBits2::eMemoryWrite,
            .srcStageMask  = vk::PipelineStageFlagBits2::eAllCommands,
            .dstAccessMask = vk::AccessFlagBits2::eMemoryWrite | vk::AccessFlagBits2::eMemoryRead,
            .dstStageMask  = vk::PipelineStageFlagBits2::eAllCommands,
            .oldLayout     = vk::ImageLayout::eUndefined,
            .newLayout     = vk::ImageLayout::eGeneral,
            .aspectFlag    = vk::ImageAspectFlagBits::eColor,
            .mipLevels     = 1,
            .layerCount    = 1
        };
        VulkanImage::transitionImageLayout(context.swapChain->getImages()[context.swapChainImageIndex],
                                           transitionConfig,
                                           context.graphicsCommandPool->getBaseCommandBuffer(bufferHandle));

        const float flash = std::abs(std::sin(_frameNumber / 120.0f));
        const vk::ClearColorValue clearColor{ .float32 = { { 0.0f, 0.0f, flash, 1.0f } } };
        vk::ImageSubresourceRange subRes{ .aspectMask     = vk::ImageAspectFlagBits::eColor,
                                          .baseMipLevel   = 0,
                                          .levelCount     = 1,
                                          .baseArrayLayer = 0,
                                          .layerCount     = 1 };
        const vk::RenderingAttachmentInfo attachmentInfo{
            .imageView        = context.swapChain->getImageViews()[context.swapChainImageIndex],
            .imageLayout      = vk::ImageLayout::eGeneral, // TODO: Update to colorattachmentoptiaml
            .resolveMode      = vk::ResolveModeFlagBits::eNone,
            .resolveImageView = nullptr,
            .loadOp           = vk::AttachmentLoadOp::eClear,
            .storeOp          = vk::AttachmentStoreOp::eStore,
            .clearValue       = clearColor
        };

        const vk::RenderingInfo renderInfo{
            .renderArea           = { .offset = { .x = 0, .y = 0 }, .extent = context.swapChainExtent },
            .layerCount           = 1,
            .colorAttachmentCount = 1,
            .pColorAttachments    = &attachmentInfo,
            .pDepthAttachment     = nullptr // TODO:
        };

        context.graphicsCommandPool->getBaseCommandBuffer(bufferHandle).beginRendering(renderInfo);

        // Set dynamic states
        context.graphicsCommandPool->getBaseCommandBuffer(bufferHandle)
            .setViewport(0,
                         vk::Viewport(0.0f, static_cast<float>(context.swapChainExtent.height),
                                      static_cast<float>(context.swapChainExtent.width),
                                      -static_cast<float>(context.swapChainExtent.height), 0.0f, 1.0f));
        context.graphicsCommandPool->getBaseCommandBuffer(bufferHandle)
            .setScissor(0, vk::Rect2D(vk::Offset2D(0, 0), context.swapChainExtent));

        // TODO: Impl
        return true;
    }


    bool VulkanBackend::endFrame(float deltaTime) noexcept
    {
        const auto bufferHandle = context.graphicsCommandBuffers[context.frameIndex];


        // End the rendering
        context.graphicsCommandPool->getBaseCommandBuffer(bufferHandle).endRendering();

        // Transition the image back
        // TODO: Update transition flags
        constexpr VulkanLayoutTransitionConfig transitionConfig{
            .srcAccessMask = vk::AccessFlagBits2::eMemoryWrite,
            .srcStageMask  = vk::PipelineStageFlagBits2::eAllCommands,
            .dstAccessMask = vk::AccessFlagBits2::eMemoryWrite | vk::AccessFlagBits2::eMemoryRead,
            .dstStageMask  = vk::PipelineStageFlagBits2::eAllCommands,
            .oldLayout     = vk::ImageLayout::eGeneral,
            .newLayout     = vk::ImageLayout::ePresentSrcKHR,
            .aspectFlag    = vk::ImageAspectFlagBits::eColor,
            .mipLevels     = 1,
            .layerCount    = 1
        };
        VulkanImage::transitionImageLayout(context.swapChain->getImages()[context.swapChainImageIndex],
                                           transitionConfig,
                                           context.graphicsCommandPool->getBaseCommandBuffer(bufferHandle));

        // End recording
        context.graphicsCommandPool->endRecording(bufferHandle);
        context.graphicsCommandPool->submit(bufferHandle);

        // Submit the queue
        const vk::SubmitInfo submitInfo{ .waitSemaphoreCount = 1,
                                         .pWaitSemaphores    = &*context.imageAvailableSemaphores[context.frameIndex],
                                         .commandBufferCount = 1,
                                         .pCommandBuffers =
                                             &*context.graphicsCommandPool->getBaseCommandBuffer(bufferHandle),
                                         .signalSemaphoreCount = 1,
                                         .pSignalSemaphores    = &*context.queueCompleteSemaphore[context.frameIndex] };

        // TODO: Refactor
        context.device->getGraphicsQueue().submit(submitInfo,
                                                  context.inFlightFences[context.frameIndex].getBaseFence());

        // Submit the graphics queue

        context.swapChain->presentSwapChain(context, context.device->getGraphicsQueue(),
                                            context.device->getPresentQueue(),
                                            context.queueCompleteSemaphore[context.frameIndex], context.frameIndex);

        context.frameIndex = (context.frameIndex + 1) % context.swapChain->getMaxFramesInFlight();


        // TODO: Impl
        return true;
    }



    /// +==================================+
    ///           PRIVATE METHODS
    /// +==================================+

    Vector<const char*> VulkanBackend::getRequiredExtensions() const noexcept
    {
        auto extensions = platform::getRequiredExtensions();
        // Setup up the debug callback extension
        if (ENABLE_VALIDATION_LAYERS)
            extensions.push_back(vk::EXTDebugUtilsExtensionName);
        return extensions;
    }

    void VulkanBackend::createGraphicsCommandPoolAndAllocateBuffers() const noexcept
    {
        const auto rawMem           = memory::alloc(sizeof(VulkanCommandPool), memory::MemoryTag::MEMORY_TAG_RENDERER);
        context.graphicsCommandPool = new (rawMem) VulkanCommandPool();
        context.graphicsCommandPool->createCommandPool(context, context.device->getQueueInfo().graphicsFamilyIndex);
        log::info("Command pool for Graphics Family created!");
        /// Create a command buffer for each swapchain image.
        for (size_t i = 0; i < context.swapChain->getImageCount(); ++i)
        {
            context.graphicsCommandBuffers.push_back(context.graphicsCommandPool->allocate(context, true));
        }
        log::info("Command buffers allocated for Graphics Family!");
    }


    vk::Bool32 debugCallback(const vk::DebugUtilsMessageSeverityFlagBitsEXT severity,
                             const vk::DebugUtilsMessageTypeFlagsEXT type,
                             const vk::DebugUtilsMessengerCallbackDataEXT* pCallbackData,
                             [[maybe_unused]] void* pUserData)
    {
        const auto message =
            std::format("Validation Layer(type: {})\nMessage:\n{}\n", vk::to_string(type), pCallbackData->pMessage);
        if (severity >= vk::DebugUtilsMessageSeverityFlagBitsEXT::eError)
        {
            log::error(message.c_str());
        }
        else if (severity >= vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning)
        {
            log::warn(message.c_str());
        }
        else
        {
            log::info(message.c_str());
        }
        return vk::False;
    }


    void setupDebugMessenger() noexcept
    {
        if constexpr (ENABLE_VALIDATION_LAYERS)
        {
            log::info("Vulkan validation layers are enabled!");
            constexpr vk::DebugUtilsMessageSeverityFlagsEXT severityFlags{
                vk::DebugUtilsMessageSeverityFlagBitsEXT::eError | vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning
            };
            constexpr vk::DebugUtilsMessageTypeFlagsEXT messageType{
                vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance | vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation |
                vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral
            };

            constexpr vk::DebugUtilsMessengerCreateInfoEXT debugUtilsCreateInfo{ .messageSeverity = severityFlags,
                                                                                 .messageType     = messageType,
                                                                                 .pfnUserCallback = &debugCallback };
            context.debugMessenger =
                context.instance.createDebugUtilsMessengerEXT(debugUtilsCreateInfo, context.allocator);
        }
    }


} // namespace tempest::renderer