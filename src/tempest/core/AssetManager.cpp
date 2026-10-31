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

namespace tempest::core
{
    void AssetManager::load(std::string_view asset) noexcept
    {
        log::info(std::format("Loading asset {}", asset));
    }
} // namespace tempest::core
