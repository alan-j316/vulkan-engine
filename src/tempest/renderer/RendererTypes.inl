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

namespace tempest::renderer
{
    enum class RendererType : uint8_t
    {
        VULKAN, // Can be used to account for other renderers
        INVALID_BACKEND = 0xFF
    };


    /// Simple data packet for rendering.
    struct RenderPacket
    {
        float deltaTime;
    };
} // namespace tempest::renderer