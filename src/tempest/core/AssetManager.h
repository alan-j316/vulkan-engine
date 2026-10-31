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

#include <string_view>

namespace tempest::core
{
    class AssetManager
    {
    public:
        void load(std::string_view asset) noexcept;

    private:
        // TODO: Impl
    };
} // namespace tempest::core