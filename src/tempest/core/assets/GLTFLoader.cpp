/**
 * @file GLTFLoader.cpp
 * @author Alan Abraham P Kochumon
 * @date Created on: November 03, 2026
 *
 * @brief $BRIEF$
 *
 * @copyright Copyright (c) 2026 Alan Abraham P Kochumon
 */

#include "GLTFLoader.h"

#include "core/Logger.h"

#include <fastgltf/core.hpp>
#include <fastgltf/types.hpp>

namespace tempest::core
{
    Geometry GLTFLoader::load(const std::filesystem::path& path)
    {
        fastgltf::Parser _parser; // NOT threadsafe
        // Create a GLTF Buffer for loading the data.
        auto data = fastgltf::GltfDataBuffer::FromPath(path);
        if (data.error() != fastgltf::Error::None)
        {
            log::error(std::format("There was an error loading the GLTF: {}", path.string()));
            return {};
        }

        // Load the file and parse the JSON
        auto asset = _parser.loadGltf(data.get(), path.parent_path(), fastgltf::Options::None);
        if (auto error = asset.error(); error != fastgltf::Error::None)
        {
            log::error("There was an error reading the GLTF buffer.");
            return {};
        }

        // Validate the GLTF spec enforcement. TODO: Enable in debug
        // fastgltf::validate(asset.get());
        // for (const auto& mesh : asset.get().meshes)
        // {
        //     for (const auto& primitive : mesh.primitives)
        //     {
        //         const auto positionIt  = primitive.findAttribute("POSITION");
        //         auto& positionAccessor = asset->accessors[positionIt->accessorIndex];
        //         if (!positionAccessor.bufferViewIndex.has_value())
        //             continue;
        //         auto* vertices = static_cast<Vertex*>(glMapNamedBuffer(primitive.vertexBuffer, GL_WRITE_ONLY));
        //         fastgltf::iterateAccessorWithIndex<fastgltf::math::fvec3>(
        //             asset, positionAccessor, [&](fastgltf::math::fvec3 pos, std::size_t idx) {
        //                 vertices[idx].position = fastgltf::math::fvec3(pos.x(), pos.y(), pos.z());
        //                 vertices[idx].uv       = fastgltf::math::fvec2();
        //             });
        //     }
        // }
    return {};
    }


} // namespace tempest::core
