#pragma once
/**
 * @file Utils.h
 * @author Alan Abraham P Kochumon
 * @date Created on: October 30, 2026
 *
 * @brief Utils for vulkan renderer.
 *
 * @copyright Copyright (c) 2026 Alan Abraham P Kochumon
 */


#include "core/Vertex.h"
#include "renderer/vulkan/VulkanTypes.inl"

#include <array>

namespace tempest::renderer
{
    template <typename T>
    constexpr vk::VertexInputBindingDescription getBindingDescription(const size_t bindingIndex,
                                                                      const vk::VertexInputRate inputRate)
    {
        return vk::VertexInputBindingDescription{ .binding   = static_cast<uint32_t>(bindingIndex),
                                                  .stride    = sizeof(T),
                                                  .inputRate = inputRate };
    }


    constexpr std::array<vk::VertexInputAttributeDescription, 2> getAttributeDescription()
    {
        return std::array{ vk::VertexInputAttributeDescription{ .location = 0,
                                                                .binding  = 0,
                                                                .format   = vk::Format::eR32G32B32Sfloat,
                                                                .offset   = offsetof(core::Vertex, position) },
                           vk::VertexInputAttributeDescription{ .location = 1,
                                                                .binding  = 0,
                                                                .format   = vk::Format::eR32G32B32Sfloat,
                                                                .offset   = offsetof(core::Vertex, color) } };
    }

} // namespace tempest::renderer