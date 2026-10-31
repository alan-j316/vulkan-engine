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

#include <print> // TODO: Remove

namespace fps
{
    bool FPSGame::initialize()
    {
        tempest::log::info("Game initialized");

        auto assertMgr = getContext().assetManager;
        assertMgr.load("assets/buggy.gltf");

        return true;
    }

    bool FPSGame::update(float deltaTime) { return true; }

    bool FPSGame::render(float deltaTime) { return true; }

    void FPSGame::onResize(uint32_t width, uint32_t height) {  }
} // namespace fps
