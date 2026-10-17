/**
 * @file FPSGame.cpp
 * @author Alan Abraham P Kochumon
 * @date Created on: October 17, 2026
 *
 * @brief Implementation of member functions declared in FPSGame.h
 *
 * @copyright Copyright (c) 2026 Alan Abraham P Kochumon
 */

#include "FPSGame.h"

namespace fps
{
    bool FPSGame::initialize()
    {
        tempest::log::info("Game initialized");
        return true;
    }

    bool FPSGame::update(float deltaTime)
    {
        tempest::log::info("Game updating");
        return true;
    }

    bool FPSGame::render(float deltaTime) { return true; }

    void FPSGame::onResize(uint32_t width, uint32_t height) {}
} // namespace fps
