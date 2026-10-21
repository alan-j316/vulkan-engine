/**
 * @file Clock.cpp
 * @author Alan Abraham P Kochumon
 * @date Created on: October 21, 2026
 *
 * @brief Implementation of member functions declared in Clock.h
 *
 * @copyright Copyright (c) 2026 Alan Abraham P Kochumon
 */


#include "Clock.h"
#include "platform/Time.h"


namespace tempest::core
{
    void Clock::start() noexcept
    {
        elapsedTime = 0.0;
        startTime = platform::getAbsoluteTime();
    }

    void Clock::stop() noexcept
    {
        startTime = 0.0;
    }

    void Clock::update() noexcept
    {
        if (startTime > 0)
        {
            elapsedTime = platform::getAbsoluteTime() - startTime;
        }
    }

} // namespace tempest::core