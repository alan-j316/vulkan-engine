#pragma once
/**
 * @file Entry.h
 * @author Alan Abraham P Kochumon
 * @date Created on: October 17, 2026
 *
 * @brief Entry Point.
 *
 * @copyright Copyright (c) 2026 Alan Abraham P Kochumon
 */

#include "Game.h"
#include "MemorySubsystem.h"
#include "core/Logger.h"



int main()
{
    // Application
    tempest::core::Application application;
    if (!application.create())
    {
        tempest::log::error("Failed to create application");
        return 1;
    }

    if (!application.run())
    {
        tempest::log::error("Application did not shutdown gracefully!");
        return 2;
    }
}