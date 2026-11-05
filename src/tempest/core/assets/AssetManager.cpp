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
    bool AssetManager::init(renderer::RendererEventQueue* rendererEventQueue) noexcept
    {
        _rendererEventQueue = rendererEventQueue;
        return true;
    }


    void AssetManager::loadModel(std::string_view asset) noexcept
    {
        log::info(std::format("Loading asset {}", asset));
        auto loadedAsset = _gltfLoader.load(std::filesystem::path(asset));
        renderer::RenderUploadEvent event{ .vertices = std::move(loadedAsset.vertices),
                                           .indices  = std::move(loadedAsset.indices) };
        _rendererEventQueue->push(event);
    }
} // namespace tempest::core
