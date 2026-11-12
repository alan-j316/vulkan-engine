#pragma once
/**
 * @file AppConfig.h
 * @author Alan Abraham P Kochumon
 * @date Created on: October 17, 2026
 *
 * @brief Application configuration.
 *
 * @copyright Copyright (c) 2026 Alan Abraham P Kochumon
 */

#include <cstdint>
#include <string>

namespace tempest
{
    struct AppConfig
    {
        uint32_t startPosX;
        uint32_t startPosY;
        uint32_t width;
        uint32_t height;
        std::string name;
    };
} // namespace tempest