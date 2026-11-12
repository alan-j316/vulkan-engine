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

    struct ModelUniform
    {
        flcn::Mat4F model;
    };


    /// Simple data packet for rendering.
    struct RenderPacket
    {
        float deltaTime;
    };
} // namespace tempest::renderer