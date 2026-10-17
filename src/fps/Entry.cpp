/**
 * @file FPSGame.cpp
 * @author Alan Abraham P Kochumon
 * @date Created on: October 14, 2026
 *
 * @brief Game application entry point.
 *
 * @copyright Copyright (c) 2026 Alan Abraham P Kochumon
 */

#include "Application.h"
#include "FPSGame.h"

#include <Entry.h>
#include <cstdlib>
#include <new>
#include <print>


tempest::Game* createGame()
{
    const auto fpsGame = new (tempest::platform::alloc(sizeof(fps::FPSGame))) fps::FPSGame();
    fpsGame->setConfig({ .startPosX = 100, .startPosY = 100, .width = 1280, .height = 720, .name = "FPS Game" });
    return fpsGame;
}


extern bool destroyGame(tempest::Game* game)
{
    if (!game)
        return false;
    game->~Game();
    tempest::platform::free(game, sizeof(fps::FPSGame));

    return true;
}
