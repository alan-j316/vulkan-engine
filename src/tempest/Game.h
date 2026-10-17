#pragma once
/**
 * @file GameTypes.h
 * @author Alan Abraham P Kochumon
 * @date Created on: October 17, 2026
 *
 * @brief Game Types.
 *
 * @copyright Copyright (c) 2026 Alan Abraham P Kochumon
 */

#include "core/AppConfig.h"
#include "core/Application.h"

namespace tempest
{
    class Game
    {
    public:
        virtual ~Game() = default;
        virtual bool initialize() { return false; }
        virtual bool update(float deltaTime) { return false; }
        virtual bool render(float deltaTime) { return false; }
        virtual void onResize(uint32_t width, uint32_t height) {}

        [[nodiscard]] const AppConfig& getConfig() const { return _config; }
        [[nodiscard]] AppConfig& getConfig() { return _config; }

        void setConfig(const AppConfig& config) { _config = config; }

    private:
        void* _state{};
        AppConfig _config{};
    };
} // namespace tempest