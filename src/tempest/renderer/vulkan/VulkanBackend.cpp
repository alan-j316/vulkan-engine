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

#include "VulkanBuffer.h"
#include "VulkanCmdPool.h"
#include "VulkanGraphicsPipeline.h"
#include "VulkanTypes.inl"
#include "core/Asserts.h"
#include "core/FileReader.h"
#include "core/Logger.h"
#include "core/MemorySubsystem.h"
#include "core/assets/Asset.h"
#include "core/assets/AssetManager.h"
#include "platform/Platform.h"
#include "renderer/vulkan/VulkanDevice.h"
#include "renderer/vulkan/VulkanPlatform.h"
#include "renderer/vulkan/VulkanSwapChain.h"

#include <algorithm>
#include <iterator>

// TODO: Temporary REMOVE
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
// TODO: Temporary REMOVE END


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
        VulkanCmdPool graphicsCmdPool{};
        VulkanGraphicsPipeline graphicsPipeline{};
        Vector<VulkanCmdPool::CmdBufferHandle> graphicsCommandBuffers{ makeVector<VulkanCmdPool::CmdBufferHandle>() };
        // Synchronization primitives
        Vector<vk::raii::Semaphore> imageAvailableSemaphores{ makeVector<vk::raii::Semaphore>() };
        Vector<vk::raii::Semaphore> renderFinishedSemaphore{ makeVector<vk::raii::Semaphore>() };
        Vector<vk::raii::Fence> inFlightFences{};
        VmaAllocator vma{};
        // Vector<VulkanFence> imagesInFlight{};
        // TODO: REMOVE
        size_t activeIndices{ 0 };
        VulkanBuffer vertexBuffer{}, indexBuffer{};
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


    /////////////////////////////////////
    /// TODO: TEMPORARY METHODS REMOVE
    /////////////////////////////////////
    struct Texture
    {
        stbi_uc* pixels;
        int32_t width, height, channels;
        size_t size;
    };


    Texture readTexture()
    {
        Texture texture;
        texture.pixels = stbi_load("assets/textures/viking_room.png", &texture.width, &texture.height,
                                   &texture.channels, STBI_rgb_alpha);
        texture.size   = texture.width * texture.height * 4;

        if (texture.pixels == nullptr)
        {
            log::error("There was an error loading the image");
        }
        // NOT VERY GOOD ERROR HANDLING
        return texture;
    }
    // TODO: Temporary REMOVE END


    /// Debug Callback
    static vk::Bool32 debugCallback(vk::DebugUtilsMessageSeverityFlagBitsEXT severity,
                                    vk::DebugUtilsMessageTypeFlagsEXT type,
                                    const vk::DebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData);

    bool VulkanBackend::init(platform::PlatformState* state, std::string_view applicationName,
                             RendererEventQueue* rendererEventQueue) noexcept
    {
        const auto rawCtxMemory = memory::alloc(sizeof(VulkanContext), memory::MemoryTag::MEMORY_TAG_RENDERER);
        _ctx                    = new (rawCtxMemory) VulkanContext();
        _rendererEventQueue     = rendererEventQueue;
        _state                  = state;

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
        _ctx->swapChain.create(_ctx->device, _ctx->surface, _ctx->swapChainExtent.width, _ctx->swapChainExtent.height);


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
            constexpr vk::FenceCreateInfo fenceCreateInfo{ .flags = vk::FenceCreateFlagBits::eSignaled };
            _ctx->inFlightFences.emplace_back(_ctx->device.getLogicalDevice(), fenceCreateInfo, _ctx->allocator);
        }

        for (size_t i = 0; i < _ctx->swapChain.getImages().size(); ++i)
        {
            vk::SemaphoreCreateInfo semaphoreCreateInfo{};
            _ctx->renderFinishedSemaphore.emplace_back(_ctx->device.getLogicalDevice(), semaphoreCreateInfo,
                                                       _ctx->allocator);
        }


        // ------------------------
        //  VMA
        // ------------------------

        VmaAllocatorCreateInfo vmaInfo{ .physicalDevice       = *_ctx->device.getPhysicalDevice(),
                                        .device               = *_ctx->device.getLogicalDevice(),
                                        .pAllocationCallbacks = *_ctx->allocator,
                                        .instance             = *_ctx->instance,
                                        .vulkanApiVersion     = MIN_VULKAN_VERSION };
        vmaCreateAllocator(&vmaInfo, &_ctx->vma);


        // ------------------------
        //  PIPELINES
        // ------------------------

        // TODO: TEMPORARY
        const auto vertCode = utils::readFile("assets/shaders/Simple.vert.spv");
        const auto fragCode = utils::readFile("assets/shaders/Simple.frag.spv");
        // TODO: TEMPORARY

        _ctx->graphicsPipeline = VulkanGraphicsPipeline();
        const GraphicsPipelineConfig pipelineConfig{
            .vertShaderEntryPoint = "vertMain", // TODO: Update
            .pVertShaderCode      = vertCode,
            .fragShaderEntryPoint = "fragMain",
            .pFragShaderCode      = fragCode,
            .wireframe            = false,
            .enableMsaa           = vk::False,
            .msaaSamples          = vk::SampleCountFlagBits::e1,
            .winding              = vk::FrontFace::eClockwise,
            .cullMode             = vk::CullModeFlagBits::eNone,
            .topology             = vk::PrimitiveTopology::eTriangleList,

        };
        _ctx->graphicsPipeline.create(_ctx->device, _ctx->swapChain, pipelineConfig, _ctx->vma);



        /// TODO: TEMP REMOVE
        constexpr auto VERTEX_SIZE = 64 * 1024 * 1024;
        constexpr auto INDEX_SIZE  = 4 * 1024 * 1024;
        const BufferConfig vertexConfig{
            .size = VERTEX_SIZE,
            // .size        = vertices.size() * sizeof(core::Vertex),
            .usageFlags  = vk::BufferUsageFlagBits::eVertexBuffer,
            .property    = vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
            .sharingMode = vk::SharingMode::eExclusive,
        };
        _ctx->vertexBuffer.create(_ctx->vma, vertexConfig);

        const BufferConfig indexConfig{
            .size = INDEX_SIZE,
            // .size        = vertices.size() * sizeof(core::Vertex),
            .usageFlags  = vk::BufferUsageFlagBits::eIndexBuffer,
            .property    = vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
            .sharingMode = vk::SharingMode::eExclusive,
        };
        _ctx->indexBuffer.create(_ctx->vma, indexConfig);

        //// TEXTURE PROCESSING
        // const auto texture = readTexture();
        // VulkanBuffer texBuffer{};
        // const BufferConfig textureConfig{ .size       = texture.size,
        //                                   .usageFlags = vk::BufferUsageFlagBits::eTransferSrc,
        //                                   .property   = vk::MemoryPropertyFlagBits::eHostVisible |
        //                                       vk::MemoryPropertyFlagBits::eHostCoherent,
        //                                   .sharingMode = vk::SharingMode::eExclusive };
        // texBuffer.create(_ctx->vma, textureConfig);
        // void* data = texBuffer.map();
        // memcpy(data, texture.pixels, texture.size);
        // texBuffer.unmap();
        // stbi_image_free(texture.pixels);

        // VulkanImageCreateInfo textureImgInfo{
        //     .imageType = vk::ImageType::e2D,
        //     .width     = static_cast<uint32_t>(texture.width),
        //     .height    = static_cast<uint32_t>(texture.height),
        //     .format    = vk::Format::eR8G8B8A8Srgb,
        //     // TODO: Update property flags
        //     .usage       = vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled,
        //     .properties  = vk::MemoryPropertyFlagBits::eHostCoherent | vk::MemoryPropertyFlagBits::eHostVisible,
        //     .aspectFlags = vk::ImageAspectFlagBits::eColor,
        //     .tiling      = vk::ImageTiling::eOptimal,
        //     .createView  = true,
        //     .mipLevels   = 1,
        //     .numSamples  = vk::SampleCountFlagBits::e1
        // };
        // VulkanImage textureImg{};
        // textureImg.create(_ctx->device, textureImgInfo);
        // // Transition layout for rendering
        // const auto textureCmdId = _ctx->graphicsCmdPool.allocateAndStartSingleTimeRecording(_ctx->device);
        // _ctx->graphicsCmdPool.makeActive(textureCmdId);
        // VulkanLayoutTransitionConfig transitionConfig{ .srcAccessMask = {},
        //                                                .srcStageMask  = vk::PipelineStageFlagBits2::eTopOfPipe,
        //                                                .dstAccessMask = vk::AccessFlagBits2::eTransferWrite,
        //                                                .dstStageMask  = vk::PipelineStageFlagBits2::eTransfer,
        //                                                .oldLayout     = vk::ImageLayout::eUndefined,
        //                                                .newLayout     = vk::ImageLayout::eTransferDstOptimal,
        //                                                .aspectFlag    = vk::ImageAspectFlagBits::eColor };
        // textureImg.transitionImageLayout(vk::ImageLayout::eTransferDstOptimal,transitionConfig,
        // _ctx->graphicsCmdPool.get(textureCmdId));

        // TODO:
        // _ctx->graphicsCmdPool.endRecording();/

        // texBuffer.destroy();
        /// TODO: TEMP REMOVE

        log::info("Vulkan Renderer Initialized");
        return _ctx->instance != nullptr;
    }


    void VulkanBackend::shutdown() noexcept
    {
        // Wait for idle before shutting down sync primitives
        _ctx->device.getLogicalDevice().waitIdle();

        // TODO: REMOVE
        _ctx->vertexBuffer.destroy();
        _ctx->indexBuffer.destroy();
        // TODO: END REMOVE


        _ctx->graphicsPipeline.destroy();

        vmaDestroyAllocator(_ctx->vma);

        // Destroy sync primitives
        _ctx->imageAvailableSemaphores.clear();
        _ctx->renderFinishedSemaphore.clear();
        _ctx->inFlightFences.clear();
        _ctx->swapChain.destroy();

        _ctx->graphicsCmdPool.destroy();
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
        const auto& dev       = _ctx->device.getLogicalDevice();
        _ctx->swapChainExtent = { .width = width, .height = height };
        // if (dev.waitForFences(*_ctx->inFlightFences[_ctx->frameIndex], vk::True, UINT64_MAX) != vk::Result::eSuccess)
        // {
        //     log::error("[Resize]: Unable to wait for in-flight fence. Swap chain won't be resized!");
        //     return;
        // }
        dev.waitIdle();
        log::info(std::format("Resizing to {} x {} ", _ctx->swapChainExtent.width, _ctx->swapChainExtent.height));
        _ctx->swapChain.recreate(_ctx->device, _ctx->surface, width, height);
    }


    bool VulkanBackend::beginFrame(float deltaTime) noexcept
    {
        update();

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
        // Make the command buffer the frame index the active one
        _ctx->graphicsCmdPool.makeActive(bufferHandle);


        _ctx->graphicsCmdPool.reset();
        // NOTE: We are resetting the command buffer and re-recording it every frame.
        _ctx->graphicsCmdPool.startRecording(true, false);


        // ------------------------------
        //     PIPELINE CONFIGURATION
        // ------------------------------
        _ctx->graphicsCmdPool.bindPipeline(vk::PipelineBindPoint::eGraphics, _ctx->graphicsPipeline);

        // TODO: TEMP REMOVE
        // const auto vertSize = vertices.size() * sizeof(core::Vertex);
        // void* vertexMemory  = _ctx->vertexBuffer.map();
        // memcpy(vertexMemory, vertices.data(), vertSize);
        // _ctx->vertexBuffer.unmap();
        // _ctx->graphicsCmdPool.bindVertexBuffer(bufferHandle, 0, _ctx->vertexBuffer);
        //
        // const auto idxSize = indices.size() * sizeof(uint32_t);
        // void* indexMemory  = _ctx->indexBuffer.map();
        // memcpy(indexMemory, indices.data(), idxSize);
        // _ctx->indexBuffer.unmap();
        // _ctx->graphicsCmdPool.bindIndexBuffer(bufferHandle, 0, vk::IndexType::eUint32, _ctx->indexBuffer);

        _ctx->graphicsCmdPool.bindVertexBuffer(0, _ctx->vertexBuffer);
        _ctx->graphicsCmdPool.bindIndexBuffer(0, vk::IndexType::eUint32, _ctx->indexBuffer);
        // TODO: TEMP REMOVE

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
                                           _ctx->graphicsCmdPool.baseCmdBuffer());
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

        _ctx->graphicsCmdPool.beginRendering(renderInfo);

        // Set dynamic states
        _ctx->graphicsCmdPool.setViewport(0,
                                          vk::Viewport(0.0f, 0.0f, static_cast<float>(_ctx->swapChainExtent.width),
                                                       static_cast<float>(_ctx->swapChainExtent.height), 0.0f, 1.0f));
        _ctx->graphicsCmdPool.setScissor(0, vk::Rect2D(vk::Offset2D(0, 0), _ctx->swapChainExtent));
        _ctx->graphicsCmdPool.bindDescriptorSet(vk::PipelineBindPoint::eGraphics,
                                                _ctx->graphicsPipeline.descriptorLayout(), 0,
                                                _ctx->graphicsPipeline.descriptorSet(_ctx->frameIndex), nullptr);
        _ctx->graphicsCmdPool.drawIndexed(static_cast<uint32_t>(_ctx->activeIndices), 1, 0, 0, 0);
        //TODO: Needs to put the binding right here

        return true;
    }


    bool VulkanBackend::endFrame(float deltaTime) noexcept
    {
        const auto bufferHandle = _ctx->graphicsCommandBuffers[_ctx->frameIndex];

        _ctx->graphicsCmdPool.makeActive(bufferHandle);

        // End the rendering
        _ctx->graphicsCmdPool.endRendering();

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
                                           _ctx->graphicsCmdPool.baseCmdBuffer());

        // End recording
        _ctx->graphicsCmdPool.endRecording();
        _ctx->graphicsCmdPool.submit();

        // Submit the queue
        vk::PipelineStageFlags waitDstFlag{ vk::PipelineStageFlagBits::eColorAttachmentOutput };
        const vk::SubmitInfo submitInfo{ .waitSemaphoreCount   = 1,
                                         .pWaitSemaphores      = &*_ctx->imageAvailableSemaphores[_ctx->frameIndex],
                                         .pWaitDstStageMask    = &waitDstFlag,
                                         .commandBufferCount   = 1,
                                         .pCommandBuffers      = &*_ctx->graphicsCmdPool.baseCmdBuffer(),
                                         .signalSemaphoreCount = 1,
                                         .pSignalSemaphores =
                                             &*_ctx->renderFinishedSemaphore[_ctx->swapChainImageIndex] };
        _ctx->device.getGraphicsQueue().submit(submitInfo, _ctx->inFlightFences[_ctx->frameIndex]);

        // Present swap chain
        _ctx->swapChain.presentSwapChain(
            _ctx->device, _ctx->surface, _ctx->device.getGraphicsQueue(), _ctx->device.getPresentQueue(),
            _ctx->renderFinishedSemaphore[_ctx->swapChainImageIndex], _ctx->swapChainImageIndex);

        _ctx->frameIndex = (_ctx->frameIndex + 1) % _ctx->swapChain.getMaxFramesInFlight();

        return true;
    }


    bool VulkanBackend::uploadGeometry(const core::Geometry& geometry)
    {
        // TODO: Update
        // std::copy(geometry.vertices.begin(), geometry.vertices.end(), std::back_inserter(vertices));
        // std::copy(geometry.indices.begin(), geometry.indices.end(), std::back_inserter(indices));
        // const auto bufferHandle = _ctx->graphicsCommandBuffers[_ctx->frameIndex];
        // _ctx->graphicsCommandPool.bindPipeline(bufferHandle, vk::PipelineBindPoint::eGraphics,
        // _ctx->graphicsPipeline); const auto vertSize = geometry.vertices.size() * sizeof(core::Vertex); void*
        // vertexMemory  = _ctx->vertexBuffer.map(); memcpy(vertexMemory, vertices.data(), vertices.size() *
        // sizeof(core::Vertex)); _ctx->vertexBuffer.unmap(); _ctx->graphicsCommandPool.bindVertexBuffer(bufferHandle,
        // 0, _ctx->vertexBuffer);
        return true;
    }


    bool VulkanBackend::updateGlobalState(flcn::Mat4F projection, flcn::Mat4F view, flcn::Vec3F viewPosition,
                                          flcn::Vec4F ambientColor, uint32_t mode) noexcept
    {
        const auto cmdBufferHandle = _ctx->graphicsCommandBuffers[_ctx->frameIndex];
        _ctx->graphicsCmdPool.makeActive(cmdBufferHandle);
        _ctx->graphicsPipeline.updateGlobalUBO(_ctx->frameIndex, GlobalUniform{ .view = view, .proj = projection });
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
        _ctx->graphicsCmdPool.createCommandPool(_ctx->device, _ctx->device.getQueueInfo().graphicsFamilyIndex);
        log::info("Command pool for Graphics Family created!");
        /// Create a command buffer for each swapchain image.
        for (size_t i = 0; i < _ctx->swapChain.getImageCount(); ++i)
        {
            _ctx->graphicsCommandBuffers.push_back(_ctx->graphicsCmdPool.allocate(_ctx->device, true));
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



    /// +---------------------------------------------------------------------------------------+
    /// |                                UPDATE                                                 |
    /// +---------------------------------------------------------------------------------------+
    /// TODO: Temporary buffer handle pass (REMOVE after making active buffer in place).
    void VulkanBackend::update() noexcept
    {
        // const auto bufferHandle = _ctx->graphicsCommandBuffers[_ctx->frameIndex];

        RenderUploadEvent event{};
        while (_rendererEventQueue->poll(event))
        {
            const auto& vertices = event.vertices;
            const auto& indices  = event.indices;

            // Upload vertex data.
            const auto vertSize = vertices.size() * sizeof(core::Vertex);
            void* vertexMemory  = _ctx->vertexBuffer.map();
            memcpy(vertexMemory, vertices.data(), vertSize);
            _ctx->vertexBuffer.unmap();

            // Upload index data.
            const auto idxSize = indices.size() * sizeof(uint32_t);
            void* indexMemory  = _ctx->indexBuffer.map();
            memcpy(indexMemory, indices.data(), idxSize);
            _ctx->indexBuffer.unmap();
            _ctx->activeIndices = indices.size();
        }
    }


} // namespace tempest::renderer