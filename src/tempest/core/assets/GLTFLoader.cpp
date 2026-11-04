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

#include "../../../../out/build/MSVC-Release/_deps/fastgltf-src/include/fastgltf/tools.hpp"
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
        std::vector<Vertex> vertices;

        // Validate the GLTF spec enforcement. TODO: Enable in debug
        // fastgltf::validate(asset.get());

        // Iterate through each meshes and populate vertices
        // TODO: Now only a single primitive is populated
        for (const auto& mesh : asset.get().meshes)
        {
            for (const auto& primitive : mesh.primitives)
            {
                const auto positionIt  = primitive.findAttribute("POSITION");
                auto& positionAccessor = asset->accessors[positionIt->accessorIndex];
                if (!positionAccessor.bufferViewIndex.has_value())
                    continue;
                vertices.resize(positionAccessor.count);
                fastgltf::iterateAccessorWithIndex<fastgltf::math::fvec3>(
                    asset.get(), positionAccessor,
                    [&vertices](fastgltf::math::fvec3 position, const size_t vertexIndex) noexcept {
                        vertices[vertexIndex].position =
                            Vec3{ .x = position.x(), .y = position.y(), .z = position.z() };
                    });
            }
        }
        return { .vertices = vertices };
    }


} // namespace tempest::core
