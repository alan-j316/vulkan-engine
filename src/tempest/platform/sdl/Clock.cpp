/**
 * @file Clock.cpp
 * @author Alan Abraham P Kochumon
 * @date Created on: October 17, 2026
 *
 * @brief Implementation of member functions declared in Clock.h
 *
 * @copyright Copyright (c) 2026 Alan Abraham P Kochumon
 */


#include "platform/Clock.h"

#include <SDL3/SDL.h>


namespace tempest::platform
{
    double getAbsoluteTime() { return SDL_GetTicks() / 1000.0f; }

    void sleep(const size_t ms) { SDL_Delay(static_cast<uint32_t>(ms)); }
} // namespace tempest::platform