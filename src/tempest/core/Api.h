#pragma once
/**
 * @file Api.h
 * @author Alan Abraham P Kochumon
 * @date Created on: October 14, 2026
 *
 * @brief API Specific declarations.
 *
 * @copyright Copyright (c) 2026 Alan Abraham P Kochumon
 */

/**
 * @brief API directive for handle DLL exports.
 */
#ifdef TEMPEST_DLL
    #ifdef _WIN32
        #ifdef TEMPEST_LIB_BUILD
            #define TEMPEST_API __declspec(dllexport)
        #else
            #define TEMPEST_API __declspec(dllimport)
        #endif
    #else
        #if __GNUC__ >= 4 || defined(__clang__)
            #define TEMPEST_API __attribute__((visibility("default")))
        #else
            #define TEMPEST_API
        #endif
    #endif
#else
    #define TEMPEST_API
#endif
