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

        auto assertMgr = getContext().assetManager;
        // assertMgr.loadModel("assets/models/monkey.glb");
        assertMgr.loadModel("assets/models/viking_room.glb");

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

        return true;
    }

    bool FPSGame::render(float deltaTime) { return true; }

    void FPSGame::onResize(uint32_t width, uint32_t height) {  }
} // namespace fps
