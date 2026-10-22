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

namespace tempest
{
    namespace renderer
    {
        /// Forward Declaration
        struct VulkanContext;

    } // namespace renderer


    namespace platform
    {
        /// Forward Declaration
        struct PlatformState;

        static Vector<const char*> getRequiredExtensions();

        static bool createVulkanSurface(const PlatformState& platformState, renderer::VulkanContext& context);
    } // namespace platform
} // namespace tempest