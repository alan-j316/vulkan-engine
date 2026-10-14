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

#include <print>

namespace tempest
{
    bool TempestEngine::startup() noexcept
    {
        std::println("Starting up the engine");
        return true;
    }

    void TempestEngine::run() noexcept
    {
        std::println("Engine is running");
    }


    void TempestEngine::shutdown() noexcept
    {
        std::println("Shutting donw the engine");
    }
} // namespace tempest
