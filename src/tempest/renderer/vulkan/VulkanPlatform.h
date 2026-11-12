#pragma once
/**
 * @file VulkanPlatform.h
 * @author Alan Abraham P Kochumon
 * @date Created on: October 22, 2026
 *
 * @brief Vulkan specific platform declarations.
 *
 * @copyright Copyright (c) 2026 Alan Abraham P Kochumon
 */

#include "containers/Vector.h"

/// Forward Declaration
namespace vk::raii
{
    class Instance;
    class SurfaceKHR;
} // namespace vk::raii

namespace tempest::platform
{
    /// Forward Declaration
    struct PlatformState;

    Vector<const char*> getRequiredExtensions();

    vk::raii::SurfaceKHR createVulkanSurface(const PlatformState& platformState, const vk::raii::Instance& instance);
} // namespace tempest::platform