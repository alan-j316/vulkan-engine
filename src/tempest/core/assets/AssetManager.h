#pragma once
/**
 * @file AssetManager.h
 * @author Alan Abraham P Kochumon
 * @date Created on: October 30, 2026
 *
 * @brief Asset manager.
 *
 * @copyright Copyright (c) 2026 Alan Abraham P Kochumon
 */

#include "GLTFLoader.h"
#include "renderer/events/RendererEventQueue.h"

#include <string_view>
#include <vector>

namespace tempest::core
{
    using AssetHandle = uint32_t;

    class AssetManager
    {
    public:
        bool init(renderer::RendererEventQueue* rendererEventQueue) noexcept;
        void loadModel(std::string_view asset) noexcept;

    private:
        GLTFLoader _gltfLoader;


        renderer::RendererEventQueue* _rendererEventQueue;

        std::vector<Vertex> _vertices;
        std::vector<AssetHandle> _vertexHandles;
        std::vector<AssetHandle> _vertexLookup;

        std::vector<uint32_t> _indices;
        std::vector<AssetHandle> _indexHandles;
        std::vector<AssetHandle> _indexLookup; /// Required for reverse lookup

        struct Mesh
        {
            AssetHandle vertices;
            AssetHandle indices;
        };


        friend GLTFLoader;

        // std::vector<AssetHandle> _geometryHandles;
        // std::vector<Geometry> _geometries;
        // TODO: Impl
    };
} // namespace tempest::core