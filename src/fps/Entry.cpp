/**
 * @file FPSGame.cpp
 * @author Alan Abraham P Kochumon
 * @date Created on: October 14, 2026
 *
 * @brief Game application entry point.
 *
 * @copyright Copyright (c) 2026 Alan Abraham P Kochumon
 */

#include "FPSGame.h"

#include <Tempest.h>
#include <Entry.h>
#include <cstdlib>
#include <new>
#include <print>


tempest::Game* createGame()
{
    const auto fpsGame =
        new (tempest::alloc(sizeof(fps::FPSGame), tempest::MemoryTag::MEMORY_TAG_GAME)) fps::FPSGame();
    fpsGame->setConfig({ .startPosX = 100, .startPosY = 100, .width = 1280, .height = 720, .name = "FPS Game" });
    return fpsGame;
}


bool destroyGame(tempest::Game* game)
{
    if (!game)
        return false;
    game->~Game();
    tempest::free(game, sizeof(fps::FPSGame), tempest::MemoryTag::MEMORY_TAG_GAME);
    return true;
}
