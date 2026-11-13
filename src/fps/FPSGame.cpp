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

#include <core/Logger.h>
#include <print> // TODO: Remove

namespace fps
{
    bool FPSGame::initialize()
    {
        // Psuedocode
        // const auto player = registry.createEntity();
        // registry.add<MeshComponent>(player, assetManager.load("assets/models/player.glb"));
        // registry.add<HealthComponent>(player);
        // const auto weapon = registry.createEntity();
        // registry.add<DamageComponent>(weapon);
        // const auto gunAsset = registry.createEntity();
        // registry.add<MeshComponent>(weapon, );
        // registry.add<DamageComponent>(weapon, );
        // registry.add<MoveComponent>(weapon, );
        // registry.add<PositionComponent>(weapon, flcn::Vec3{0, 0, 0});
        //
        //
        // Entity player = context.createEntity();
        // context.a
        tempest::log::info("Game initialized");

        const auto assertMgr = getContext().assetManager;
        // assertMgr.loadModel("assets/models/monkey.glb");
        assertMgr->loadModel("assets/models/viking_room.glb");


        cameraPosition = flcn::Vec3F{ 0, 0, 4.0f };
        cameraRotation = flcn::Vec3F::zero();


        registerListener(static_cast<EventCode>(SystemEventCode::KEY_PRESSED), &_keyListener);
        registerListener(static_cast<EventCode>(SystemEventCode::KEY_RELEASED), &_keyListener);
        return true;
    }

    bool FPSGame::update(float deltaTime)
    {
        // if (tonicDrank)
        // while(gameEvents.collect(&e))
        //  if(typeof(e) == "Tonic Drank")
        //  fortificationSystem.update()
        //  physicsSystem.update()...
        //      fortificationSystem.update
        // registry.get<HealthComponent>().health = 1000;
        // TODO: TEMP REMOVE
        recalculateViewMatrix();
        getContext().renderer->setView(view);
        // TODO: TEMP REMOVE END
        return true;
    }

    bool FPSGame::render(float deltaTime) { return true; }

    void FPSGame::onResize(uint32_t width, uint32_t height) {}

    // TODO: REMOVE TEMP
    void FPSGame::recalculateViewMatrix() noexcept
    {
        if (isCameraDirty)
        {
            const flcn::Mat4F rotation = flcn::Mat4F::makeRotationX(cameraRotation.x()) *
                flcn::Mat4F::makeRotationY(cameraRotation.y()) * flcn::Mat4F::makeRotationZ(cameraRotation.z());
            const flcn::Mat4F translation{
                1, 0, 0, cameraPosition.x(), 0, 1, 0, cameraPosition.y(), 0, 0, 1, cameraPosition.z(), 0, 0, 0, 1,
            };
            view          = rotation * translation;
            view          = view.inverse();
            isCameraDirty = false;
        }
    }

    void FPSGame::cameraYaw(float angle)
    {
        cameraRotation.y() += angle;
        isCameraDirty = true;
    }

    void FPSGame::cameraPitch(float angle)
    {
        cameraRotation.x() += angle;
        isCameraDirty = true;

        // Gimbal Lock Prevention
        float limit = 1.569051; // 89.9 deg
        // Limit between approximately -90 and +90 deg(between limit)
        cameraRotation.x() = std::clamp(cameraRotation.x(), -limit, limit);
        info(std::format("Camera: <{:.2f}, {:.2f}, {:.2f}>", cameraRotation.x(), cameraRotation.y(),
                         cameraRotation.z()));
    }

    void FPSGame::cameraRoll(float angle)
    {

        cameraRotation.z() += angle;
        isCameraDirty = true;
    }


    bool FPSGame::KeyEventListener::onEvent(EventCode code, void* sender, EventContext context)
    {
        if (static_cast<SystemEventCode>(code) == SystemEventCode::KEY_PRESSED)
        {
            constexpr float ROTATION = 0.1f;

            const auto keyCode = static_cast<Keycode>(context.u16[0]);
            if (keyCode == Keycode::ESCAPE)
            {
                fireEvent(static_cast<EventCode>(SystemEventCode::APPLICATION_QUIT), nullptr, {});
                return false;
            }
            if (keyCode == Keycode::A)
            {
                _game.cameraYaw(ROTATION);
            }
            if (keyCode == Keycode::D)
            {
                _game.cameraYaw(-ROTATION);
            }
            if (keyCode == Keycode::W)
            {
                _game.cameraPitch(ROTATION);
            }
            if (keyCode == Keycode::S)
            {
                _game.cameraPitch(-ROTATION);
            }
            // if (keyCode == Keycode::W)
            // {
            //     _game.cameraPitch(-ROTATION);
            // }
            // if (keyCode == Keycode::S)
            // {
            //     _game.cameraPitch(-ROTATION);
            // }
        }
        else if (static_cast<SystemEventCode>(code) == SystemEventCode::KEY_RELEASED)
        {
            const auto keyCode = static_cast<Keycode>(context.u16[0]);
            if (keyCode == Keycode::C)
            {
                info("C was released");
            }
            else
            {
                // info("Another key was released");
            }
        }
        return false;
    }

    // TODO: REMOVE TEMP
} // namespace fps
