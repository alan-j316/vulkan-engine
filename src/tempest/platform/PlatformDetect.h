#pragma once
/**
 * @file PlatformDetect.h
 * @author Alan Abraham P Kochumon
 * @date Created on: October 17, 2026
 *
 * @brief Platform detection code.
 *
 * @copyright Copyright (c) 2026 Alan Abraham P Kochumon
 */

namespace tempest::platform
{
    enum PlatformType
    {
        WINDOWS,
        LINUX,
        MACOS
    };
} // namespace tempest::platform

#if defined(_WIN32) || defined(_WIN64)
inline constexpr tempest::platform::PlatformType CurrentPlatform = tempest::platform::WINDOWS;
    #define TEMPEST_PLATFORM_WINDOWS
#else

#endif