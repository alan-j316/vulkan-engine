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

#include <containers/Vector.h>
#include <cstddef>
#include <cstdint>
#include <falcon/Falcon.h>
#include <string>
#include <vector>

namespace tempest::core
{
    struct Vertex
    {
        flcn::Vec3F position;
        flcn::Vec3F color{ rand() / static_cast<float>(RAND_MAX), rand() / static_cast<float>(RAND_MAX),
                           rand() / static_cast<float>(RAND_MAX) }; // TODO: Remove
        flcn::CVec2F uv; // TODO: Update all storage types to CVec after library is updated.
    };

    struct Geometry
    {
        Vector<Vertex> vertices{ makeVector<Vertex>() };
        Vector<uint32_t> indices{ makeVector<uint32_t>() };
    };

    // using AssetHandle = uint32_t;
    //
    // struct Asset
    // {
    //     AssetHandle handle;
    //     std::string path;
    // };
} // namespace tempest::core