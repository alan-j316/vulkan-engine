/**
 * @file AssetManager.cpp
 * @author Alan Abraham P Kochumon
 * @date Created on: October 30, 2026
 *
 * @brief Implementation of functions declared in AssetManager.h
 *
 * @copyright Copyright (c) 2026 Alan Abraham P Kochumon
 */

#include "AssetManager.h"

#include "core/Logger.h"
#include "core/assets/GLTFLoader.h"

#include <format>
#include <iostream> // TODO: REMOVE


namespace tempest::core
{
    void AssetManager::loadModel(std::string_view asset) noexcept
    {
        log::info(std::format("Loading asset {}", asset));
        auto loadedAsset = _gltfLoader.load(std::filesystem::path(asset));
        // for (const auto& vert : loadedAsset.vertices)
        // {
        //     std::cout << std::format("<{}, {}, {}>\n", vert.position.x, vert.position.y, vert.position.z);
        // }
        // for (const auto& index : loadedAsset.indices)
        // {
        //     std::cout << index << ", ";
        // }
        // std::cout << std::endl;
    }
} // namespace tempest::core
