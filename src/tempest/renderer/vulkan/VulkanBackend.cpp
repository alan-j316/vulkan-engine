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
    struct VulkanContext
    {
        size_t frameIndex{ 0 }, swapChainImageIndex{ 0 };

        vk::raii::Instance instance{ nullptr };
        vk::raii::Context baseContext{};
        vk::AllocationCallbacks* allocator = nullptr; // TODO: Update with custom allocator
        vk::DebugUtilsMessengerEXT debugMessenger{};
        vk::raii::SurfaceKHR surface{ nullptr };
        vk::Extent2D swapChainExtent{ .width = 1280, .height = 720 }; // TODO: Update
        VulkanDevice device{};
        VulkanSwapChain swapChain{};
        VulkanCommandPool graphicsCommandPool{};
        Vector<CommandBufferHandle> graphicsCommandBuffers{ makeVector<CommandBufferHandle>() };
        // Synchronization primitives
        Vector<vk::raii::Semaphore> imageAvailableSemaphores{ makeVector<vk::raii::Semaphore>() };
        Vector<vk::raii::Semaphore> queueCompleteSemaphore{ makeVector<vk::raii::Semaphore>() };
        Vector<vk::raii::Fence> inFlightFences{};
        // Vector<VulkanFence> imagesInFlight{};
    };


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


    bool VulkanBackend::init(platform::PlatformState* state, std::string_view applicationName) noexcept
    {
        const auto rawCtxMemory = memory::alloc(sizeof(VulkanContext), memory::MemoryTag::MEMORY_TAG_RENDERER);
        _ctx                    = new (rawCtxMemory) VulkanContext();

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
        const auto extensionProperties = _ctx->baseContext.enumerateInstanceExtensionProperties();
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
        const auto supportedLayers = _ctx->baseContext.enumerateInstanceLayerProperties();
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

        _ctx->instance = vk::raii::Instance(_ctx->baseContext, instanceCreateInfo, _ctx->allocator);
        setupDebugMessenger();


        // -------------------
        //  SURFACE CREATION
        // -------------------
        _ctx->surface = platform::createVulkanSurface(*_state, _ctx->instance);
        if (_ctx->surface == nullptr)
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


        if (!_ctx->device.create(_ctx->instance, _ctx->surface, physicalDeviceReq))
        {
            log::error("There was an error creating the vulkan device");
            return false;
        }

        // ---------------------
        //  SWAP CHAIN CREATION
        // ---------------------
        // TODO: Update width and height
        _ctx->swapChain.create(_ctx->device, _ctx->surface, 1280, 720);


        // -------------------------
        //  COMMAND POOL AND BUFFERS
        // -------------------------
        createGraphicsCommandPoolAndAllocateBuffers();


        // ------------------------
        //  SYNCHRONIZATION OBJECTS
        // ------------------------
        _ctx->inFlightFences = makeVector<vk::raii::Fence>();
        _ctx->inFlightFences.reserve(_ctx->swapChain.getMaxFramesInFlight());

        for (size_t i = 0; i < _ctx->swapChain.getMaxFramesInFlight(); ++i)
        {
            vk::SemaphoreCreateInfo semaphoreCreateInfo{};
            _ctx->imageAvailableSemaphores.emplace_back(_ctx->device.getLogicalDevice(), semaphoreCreateInfo,
                                                        _ctx->allocator);
            _ctx->queueCompleteSemaphore.emplace_back(_ctx->device.getLogicalDevice(), semaphoreCreateInfo,
                                                      _ctx->allocator);

            constexpr vk::FenceCreateInfo fenceCreateInfo{ .flags = vk::FenceCreateFlagBits::eSignaled };
            _ctx->inFlightFences.emplace_back(_ctx->device.getLogicalDevice(), fenceCreateInfo, _ctx->allocator);
        }

        log::info("Vulkan Renderer Initialized");
        return _ctx->instance != nullptr;
    }


    void VulkanBackend::shutdown() noexcept
    {
        // Wait for idle before shutting down sync primitives
        _ctx->device.getLogicalDevice().waitIdle();

        // Destroy sync primitives
        _ctx->imageAvailableSemaphores.clear();
        _ctx->queueCompleteSemaphore.clear();
        _ctx->inFlightFences.clear();
        _ctx->swapChain.destroy();

        _ctx->graphicsCommandPool.destroy();
        _ctx->device.destroy();
        _ctx->surface  = nullptr;
        _ctx->instance = nullptr;
        // Free the renderer
        memory::free(_ctx, sizeof(VulkanContext), memory::MemoryTag::MEMORY_TAG_RENDERER);
        // NOTE: RAII will do rest of the cleanup

        log::info("Vulkan Backend successfully shutdown!");
    }

    void VulkanBackend::onResize(const uint32_t width, const uint32_t height) noexcept
    {
        _ctx->swapChainExtent = { .width = width, .height = height };
        _ctx->swapChain.recreate(_ctx->device, _ctx->surface, width, height);
    }

    //
    // void VulkanBackend::recordCommandBuffer(const uint32_t imageIndex) const noexcept
    // {
    //     auto commandBufferHandles = _ctx->graphicsCommandBuffers;
    //
    //     _ctx->graphicsCommandPool->startRecording(commandBufferHandles[_ctx->frameIndex], false, false);
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
    //         .resolveImageView   = _ctx->swapChain->getImageViews[imageIndex],
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
    //                                                                      .extent = _ctx->swapChainExtent },
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
        auto& device = _ctx->device.getLogicalDevice();

        // Wait for previous frame to be finished
        if (vk::Result::eSuccess !=
            device.waitForFences(*_ctx->inFlightFences[_ctx->frameIndex], vk::True, std::numeric_limits<size_t>::max()))
        {
            log::error("An error occurred while waiting for previous frame to finish");
            return false;
        }
        // Reset the fence
        _ctx->device.getLogicalDevice().resetFences(*_ctx->inFlightFences[_ctx->frameIndex]);

        // Acquire the next image index
        _ctx->swapChainImageIndex = _ctx->swapChain.acquireNextImageIndex(
            _ctx->device, _ctx->surface, UINT64_MAX, _ctx->imageAvailableSemaphores[_ctx->frameIndex], nullptr);



        // ------------------------------
        //     COMMAND BUFFER RECORDING
        // ------------------------------
        // Get the command buffer handle, reset and begin recording.
        const auto bufferHandle = _ctx->graphicsCommandBuffers[_ctx->frameIndex];
        _ctx->graphicsCommandPool.reset(bufferHandle);
        // NOTE: We are resetting the command buffer and re-recording it every frame.
        _ctx->graphicsCommandPool.startRecording(bufferHandle, true, false);

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
        VulkanImage::transitionImageLayout(_ctx->swapChain.getImages()[_ctx->swapChainImageIndex], transitionConfig,
                                           _ctx->graphicsCommandPool.getBaseCommandBuffer(bufferHandle));
        std::srand(std::time(nullptr));
        const auto r = rand() / static_cast<float>(RAND_MAX);
        const auto g = rand() / static_cast<float>(RAND_MAX);
        const auto b = rand() / static_cast<float>(RAND_MAX);
        // const float flash = std::abs(std::sin(_frameNumber / 120.0f));
        const vk::ClearColorValue clearColor{ .float32 = { { r, g, b, 1.0f } } };
        // vk::ImageSubresourceRange subRes{ .aspectMask     = vk::ImageAspectFlagBits::eColor,
        //                                   .baseMipLevel   = 0,
        //                                   .levelCount     = 1,
        //                                   .baseArrayLayer = 0,
        //                                   .layerCount     = 1 };
        const vk::RenderingAttachmentInfo attachmentInfo{
            .imageView        = _ctx->swapChain.getImageViews()[_ctx->swapChainImageIndex],
            .imageLayout      = vk::ImageLayout::eGeneral, // TODO: Update to colorattachmentoptiaml
            .resolveMode      = vk::ResolveModeFlagBits::eNone,
            .resolveImageView = nullptr,
            .loadOp           = vk::AttachmentLoadOp::eClear,
            .storeOp          = vk::AttachmentStoreOp::eStore,
            .clearValue       = clearColor
        };

        const vk::RenderingInfo renderInfo{
            .renderArea           = { .offset = { .x = 0, .y = 0 }, .extent = _ctx->swapChainExtent },
            .layerCount           = 1,
            .colorAttachmentCount = 1,
            .pColorAttachments    = &attachmentInfo,
            .pDepthAttachment     = nullptr // TODO:
        };

        _ctx->graphicsCommandPool.getBaseCommandBuffer(bufferHandle).beginRendering(renderInfo);

        // Set dynamic states
        _ctx->graphicsCommandPool.getBaseCommandBuffer(bufferHandle)
            .setViewport(0,
                         vk::Viewport(0.0f, static_cast<float>(_ctx->swapChainExtent.height),
                                      static_cast<float>(_ctx->swapChainExtent.width),
                                      -static_cast<float>(_ctx->swapChainExtent.height), 0.0f, 1.0f));
        _ctx->graphicsCommandPool.getBaseCommandBuffer(bufferHandle)
            .setScissor(0, vk::Rect2D(vk::Offset2D(0, 0), _ctx->swapChainExtent));

        // TODO: Impl
        return true;
    }


    bool VulkanBackend::endFrame(float deltaTime) noexcept
    {
        const auto bufferHandle = _ctx->graphicsCommandBuffers[_ctx->frameIndex];


        // End the rendering
        _ctx->graphicsCommandPool.getBaseCommandBuffer(bufferHandle).endRendering();

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
        VulkanImage::transitionImageLayout(_ctx->swapChain.getImages()[_ctx->swapChainImageIndex], transitionConfig,
                                           _ctx->graphicsCommandPool.getBaseCommandBuffer(bufferHandle));

        // End recording
        _ctx->graphicsCommandPool.endRecording(bufferHandle);
        _ctx->graphicsCommandPool.submit(bufferHandle);

        // Submit the queue
        vk::PipelineStageFlags waitDstFlag{ vk::PipelineStageFlagBits::eColorAttachmentOutput };
        const vk::SubmitInfo submitInfo{ .waitSemaphoreCount = 1,
                                         .pWaitSemaphores    = &*_ctx->imageAvailableSemaphores[_ctx->frameIndex],
                                         .pWaitDstStageMask  = &waitDstFlag,
                                         .commandBufferCount = 1,
                                         .pCommandBuffers =
                                             &*_ctx->graphicsCommandPool.getBaseCommandBuffer(bufferHandle),
                                         .signalSemaphoreCount = 1,
                                         .pSignalSemaphores    = &*_ctx->queueCompleteSemaphore[_ctx->frameIndex] };

        // TODO: Refactor
        _ctx->device.getGraphicsQueue().submit(submitInfo, _ctx->inFlightFences[_ctx->frameIndex]);

        // Present swap chain
        _ctx->swapChain.presentSwapChain(_ctx->device, _ctx->surface, _ctx->device.getGraphicsQueue(),
                                         _ctx->device.getPresentQueue(), _ctx->queueCompleteSemaphore[_ctx->frameIndex],
                                         _ctx->swapChainImageIndex);

        _ctx->frameIndex = (_ctx->frameIndex + 1) % _ctx->swapChain.getMaxFramesInFlight();


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
        // const auto rawMem         = memory::alloc(sizeof(VulkanCommandPool), memory::MemoryTag::MEMORY_TAG_RENDERER);
        // _ctx->graphicsCommandPool = new (rawMem) VulkanCommandPool();
        _ctx->graphicsCommandPool.createCommandPool(_ctx->device, _ctx->device.getQueueInfo().graphicsFamilyIndex);
        log::info("Command pool for Graphics Family created!");
        /// Create a command buffer for each swapchain image.
        for (size_t i = 0; i < _ctx->swapChain.getImageCount(); ++i)
        {
            _ctx->graphicsCommandBuffers.push_back(_ctx->graphicsCommandPool.allocate(_ctx->device, true));
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


    void VulkanBackend::setupDebugMessenger() const noexcept
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
            _ctx->debugMessenger = _ctx->instance.createDebugUtilsMessengerEXT(debugUtilsCreateInfo, _ctx->allocator);
        }
    }


} // namespace tempest::renderer