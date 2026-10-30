#pragma once
/**
 * @file Vertex.h
 * @author Alan Abraham P Kochumon
 * @date Created on: October 30, 2026
 *
 * @brief Shader vertex structure.
 *
 * @copyright Copyright (c) 2026 Alan Abraham P Kochumon
 */


namespace tempest::core
{
    struct alignas(16) Vec4
    {
        float x, y, z, w;
    };

    struct alignas(16) Vec3
    {
        float x, y, z;
    };

    struct Vertex
    {
        Vec3 position;
        Vec3 color;
    };
} // namespace tempest::core