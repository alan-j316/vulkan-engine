#pragma once
/**
 * @file FPSGame.cpp
 * @author Alan Abraham P Kochumon
 * @date Created on: October 14, 2026
 *
 * @brief First Person Shooter Game Application.
 *
 * @copyright Copyright (c) 2026 Alan Abraham P Kochumon
 */

#include <Tempest.h>

struct GameState
{};

namespace fps
{
    class FPSGame: public tempest::Game
    {
    public:
        ~FPSGame() noexcept override = default;
        bool initialize() override;
        bool update(float deltaTime) override;
        bool render(float deltaTime) override;
        void onResize(uint32_t width, uint32_t height) override;

    private:
    };
} // namespace fps