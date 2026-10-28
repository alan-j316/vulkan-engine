/**
 * @file RendererFrontend.cpp
 * @author Alan Abraham P Kochumon
 * @date Created on: October 22, 2026
 *
 * @brief Implementation of member functions declared in RendererFrontend.h
 *
 * @copyright Copyright (c) 2026 Alan Abraham P Kochumon
 */

#include "RendererFrontend.h"

#include "core/Logger.h"
#include "core/MemorySubsystem.h"
#include "renderer/RendererBackend.h"
#include "renderer/vulkan/VulkanBackend.h"

namespace tempest::renderer
{
    bool RendererFrontend::init(const RendererType type, const std::string_view applicationName,
                                platform::PlatformState* platformState) noexcept
    {
        _activeBackend = type;
        if (type == RendererType::VULKAN)
        {
            log::info("Vulkan Renderer Active!");
            const auto rawMem = memory::alloc(sizeof(VulkanBackend), memory::MemoryTag::MEMORY_TAG_RENDERER);
            _rendererBackend  = new (rawMem) VulkanBackend();
        }
        else
        {
            log::error("Unknown renderer type!");
            return false;
        }

        if (!_rendererBackend->init(platformState, applicationName))
        {
            log::error("An error occurred initializing the renderer!");
            return false;
        }

        return true;
    }


    void RendererFrontend::shutdown() noexcept
    {
        // Shutdown the renderer
        _rendererBackend->shutdown();

        // Clear the memory
        auto memSize = sizeof(RendererBackend);
        if (_activeBackend == RendererType::VULKAN)
        {
            memSize = sizeof(VulkanBackend);
        }
        memory::free(_rendererBackend, memSize, memory::MemoryTag::MEMORY_TAG_RENDERER);
        _rendererBackend = nullptr;
    }


    void RendererFrontend::onResize(const uint32_t width, const uint32_t height) const noexcept
    {
        _rendererBackend->onResize(width, height);
    }


    bool RendererFrontend::drawFrame(const RenderPacket& packet) const noexcept
    {
        // Proceed with the operation if rendering can be started.
        if (beginFrame(packet.deltaTime))
        {
            // If the frame can't be ended then the error is fatal and application must be quit.
            if (const auto result = endFrame(packet.deltaTime); !result)
            {
                log::error("Rendering failed! Application shutting down");
                return false;
            }
        }

        return true;
    }



    /// +=+=+=+=+=+=+=+=+=+=+=+=+=+=
    ///       INTERNAL METHODS
    /// +=+=+=+=+=+=+=+=+=+=+=+=+=+=

    bool RendererFrontend::beginFrame(const float deltaTime) const noexcept
    { return _rendererBackend->beginFrame(deltaTime); }


    bool RendererFrontend::endFrame(const float deltaTime) const noexcept
    {
        const auto res = _rendererBackend->endFrame(deltaTime);
        _rendererBackend->incrementFrameNumber();
        return res;
    }
} // namespace tempest::renderer