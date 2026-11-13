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
#include <core/Event.h>
#include <core/Input.h>
struct GameState
{};

namespace fps
{

    using namespace tempest::core;
    using namespace tempest::log;

    class FPSGame: public tempest::Game
    {
    public:
        ~FPSGame() noexcept override = default;
        bool initialize() override;
        bool update(float deltaTime) override;
        bool render(float deltaTime) override;
        void onResize(uint32_t width, uint32_t height) override;

        // TODO: TEMP REMOVE
        void recalculateViewMatrix() noexcept;
        void cameraYaw(float angle);
        void cameraPitch(float angle);
        void cameraRoll(float angle);


    private:
        flcn::Vec3F cameraRotation, cameraPosition;
        flcn::Mat4F view;
        bool isCameraDirty{ true };

        // TODO: TEMP REMOVE
        class KeyEventListener: public Listener
        {
        public:
            explicit KeyEventListener(FPSGame& game): _game{ game } {}
            bool onEvent(EventCode code, void* sender, EventContext context) override;

        private:
            FPSGame& _game;
        };
        KeyEventListener _keyListener{ *this };
    };
} // namespace fps