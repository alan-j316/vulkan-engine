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
#include "core/Logger.h"

extern tempest::Game* createGame();
extern bool destroyGame(tempest::Game* game);


int main()
{
    tempest::Game* game = createGame();

    // Application
    tempest::core::Application application;
    if (!application.create(game))
    {
        tempest::log::error("Failed to create application");
        return 1;
    }

    // Game loop
    if (!application.run())
    {
        tempest::log::error("Application did not shutdown gracefully!");
        return 2;
    }

    destroyGame(game);
}