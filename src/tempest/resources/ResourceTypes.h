#pragma once
/**
 * @file ResourceTypes.h
 * @author Alan Abraham P Kochumon
 * @date Created on: November 13, 2026
 *
 * @brief Types of engine resources like textures.
 *
 * @copyright Copyright (c) 2026 Alan Abraham P Kochumon
 */

#include <core/Id.h>
#include <cstddef>
#include <cstdint>

namespace tempest
{
    struct Texture
    {
        UID id;
        UID generation;
        uint32_t width, height;
        void* data;
        uint8_t channelCount;
        bool hasTransparency;
    };
} // namespace tempest