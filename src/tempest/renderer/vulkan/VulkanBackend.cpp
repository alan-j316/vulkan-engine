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

#include "VulkanTypes.inl"
#include "core/Asserts.h"
#include "core/Logger.h"
#include "core/MemorySubsystem.h"
#include "platform/Platform.h"
#include "renderer/vulkan/VulkanDevice.h"
#include "renderer/vulkan/VulkanPlatform.h"


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
        log::info("\n\n");

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
        Vector<const char*> requiredLayers;
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
        if (VulkanDevice device; !device.create(context))
        {
            log::error("There was an error creating the vulkan device");
            return false;
        }

        log::info("Vulkan Renderer Initialized");
        return context.instance != nullptr;
    }


    void VulkanBackend::shutdown() noexcept
    {
        // TODO: Impl
    }


    void VulkanBackend::onResize(uint32_t width, uint32_t height) noexcept
    {
        // TODO: Impl
    }


    bool VulkanBackend::beginFrame(float deltaTime) noexcept
    {
        // TODO: Impl
        return true;
    }


    bool VulkanBackend::endFrame(float deltaTime) noexcept
    {
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