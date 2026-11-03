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

#include <string_view>
#include <vector>

namespace tempest::core
{

    class AssetManager
    {
    public:
        void loadModel(std::string_view asset) noexcept;

    private:
        GLTFLoader _gltfLoader;

        // std::vector<AssetHandle> _geometryHandles;
        // std::vector<Geometry> _geometries;
        // TODO: Impl
    };
} // namespace tempest::core