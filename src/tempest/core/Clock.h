#pragma once
/**
 * @file Clock.h
 * @author Alan Abraham P Kochumon
 * @date Created on: October 21, 2026
 *
 * @brief System clock abstraction.
 *
 * @copyright Copyright (c) 2026 Alan Abraham P Kochumon
 */


namespace tempest::core
{
    struct Clock
    {
        void start() noexcept;
        void stop() noexcept;
        void update() noexcept;

        double startTime{}, elapsedTime{};
    };
}