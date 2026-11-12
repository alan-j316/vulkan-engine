#pragma once
/**
 * @file GLTFLoader.h
 * @author Alan Abraham P Kochumon
 * @date Created on: November 03, 2026
 *
 * @brief Class abstraction for GLTF Loader.
 *
 * @copyright Copyright (c) 2026 Alan Abraham P Kochumon
 */

#include "core/assets/Asset.h"

#include <filesystem>

namespace tempest::core
{
    class GLTFLoader
    {
    public:
        Geometry load(const std::filesystem::path& path);

    private:
    };

} // namespace tempest::core