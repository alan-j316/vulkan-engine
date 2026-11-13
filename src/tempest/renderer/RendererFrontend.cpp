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

#include "TempMath.h"
#include "core/Logger.h"
#include "core/MemorySubsystem.h"
#include "core/assets/Asset.h"
#include "renderer/RendererBackend.h"
#include "renderer/vulkan/VulkanBackend.h"

namespace tempest::renderer
{
    bool RendererFrontend::init(const RendererType type, const std::string_view applicationName,
                                platform::PlatformState* platformState, RendererEventQueue* rendererEventQueue) noexcept
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

        if (!_rendererBackend->init(platformState, applicationName, rendererEventQueue))
        {
            log::error("An error occurred initializing the renderer!");
            return false;
        }

        _projection = tmp::makePersp(1280.0f / 720.0f, flcn::constants::PI<float> / 4, _farClip, _nearClip);
        _view       = flcn::Mat4F::identity();
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


    void RendererFrontend::onResize(const uint32_t width, const uint32_t height) noexcept
    {
        _projection = tmp::makePersp(static_cast<float>(width) / static_cast<float>(height),
                                     flcn::constants::PI<float> / 4, _farClip, _nearClip);
        _rendererBackend->onResize(width, height);
    }

    bool RendererFrontend::drawFrame(const RenderPacket& packet) const noexcept
    {
        // Proceed with the operation if rendering can be started.
        if (beginFrame(packet.deltaTime))
        {
            // TODO: TEMP REMOVE
            const flcn::Mat4F model = flcn::Mat4F::makeRotationX(-flcn::constants::PI<float> / 2);
            _rendererBackend->updateObjectState(model);
            _rendererBackend->updateGlobalState(_projection, _view, flcn::Vec3F::zero(), flcn::Vec4F::zero(), 0);
            // TODO: TEMP REMOVE END
            // If the frame can't be ended then the error is fatal and application must be quit.
            if (const auto result = endFrame(packet.deltaTime); !result)
            {
                log::error("Rendering failed! Application shutting down");
                return false;
            }
        }

        return true;
    }


    bool RendererFrontend::uploadGeometry(const core::Geometry& geometry) noexcept
    { return _rendererBackend->uploadGeometry(geometry); }



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