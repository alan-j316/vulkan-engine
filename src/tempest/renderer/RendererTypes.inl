#pragma once
/**
 * @file RendererTypes.inl
 * @author Alan Abraham P Kochumon
 * @date Created on: October 22, 2026
 *
 * @brief Renderer Types
 *
 * @copyright Copyright (c) 2026 Alan Abraham P Kochumon
 */

#include <resources/ResourceTypes.h>
#include <string>


/// Forward declarations
namespace tempest::platform
{
    struct PlatformState;
}

namespace tempest::core
{
    struct Geometry;
}

namespace tempest::renderer
{
    enum class RendererType : uint8_t
    {
        VULKAN, // Can be used to account for other renderers
        INVALID_BACKEND = 0xFF
    };


    // Global Uniform Object
    struct GlobalUniform
    {
        flcn::Mat4F view;
        flcn::Mat4F proj;
        // Padded to 256-bytes to satisfy the guaranteed minUniformBufferOffsetAlignment(which is commonly to be 256)
        flcn::Mat4F _reserved0;
        flcn::Mat4F _reserved1;
    };

    struct ModelUniform // TODO: Rename this since its not really a uniform but a push constant
    {
        flcn::Mat4F model;
    };

    struct ObjectUniform
    {
        flcn::Vec4F diffuseColor;
        flcn::Vec4F reserved0, reserved1, reserved2;
    };

    struct GeometryRenderData
    {
        static constexpr size_t MAX_TEXTURES{ 16 };
        uint32_t objectId;
        flcn::Mat4F model;
        Texture* textures[MAX_TEXTURES];
    };


    /// Simple data packet for rendering.
    struct RenderPacket
    {
        float deltaTime;
    };
} // namespace tempest::renderer