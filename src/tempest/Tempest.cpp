/**
 * @file Tempest.cpp
 * @author Alan Abraham P Kochumon
 * @date Created on: October 14, 2026
 *
 * @brief Implementation of member functions declared in Tempest.h
 *
 * @copyright Copyright (c) 2026 Alan Abraham P Kochumon
 */


#include "Tempest.h"

#include "core/Logger.h"

namespace tempest
{
    TEMPEST_API bool TempestEngine::startup() noexcept
    {
        log::init();

        log::info("Starting up the engine");
        return true;
    }

    TEMPEST_API void TempestEngine::run() noexcept { log::info("Engine is running"); }


    TEMPEST_API void TempestEngine::shutdown() noexcept
    {
        log::info("Shutting down the engine");

        log::shutdown();
    }
} // namespace tempest
