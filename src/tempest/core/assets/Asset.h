#pragma once
/**
 * @file Asset.h
 * @author Alan Abraham P Kochumon
 * @date Created on: November 03, 2026
 *
 * @brief General Asset.
 *
 * @copyright Copyright (c) 2026 Alan Abraham P Kochumon
 */

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

namespace tempest::core
{
    // TODO: Swap in with falcon
    struct alignas(16) Vec3
    {
        float x, y, z;
    };

    struct Vec2
    {
        float x, y;
    };


    struct Vertex
    {
        Vec3 position;
        Vec2 uv;
    };

    struct Geometry
    {
        std::vector<Vertex> vertices;
    };

    // using AssetHandle = uint32_t;
    //
    // struct Asset
    // {
    //     AssetHandle handle;
    //     std::string path;
    // };
} // namespace tempest::core