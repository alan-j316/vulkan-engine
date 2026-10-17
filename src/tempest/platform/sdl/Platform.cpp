/**
 * @file Platform.cpp
 * @author Alan Abraham P Kochumon
 * @date Created on: October 17, 2026
 *
 * @brief Implementation of member functions declared in Platform.h
 *
 * @copyright Copyright (c) 2026 Alan Abraham P Kochumon
 */

#include "platform/Platform.h"

#include "core/Logger.h"
#include "platform/Memory.h"

#include <SDL3/SDL.h>

namespace tempest::platform
{
    struct PlatformState
    {
        SDL_Window* window{ nullptr };
    };

    void* _sdlMalloc(const size_t size) { return platform::alloc(size); }

    void* _sdlCalloc(const size_t nmemb, const size_t size)
    {
        const auto mem = platform::alloc(nmemb * size);
        return platform::setMem(mem, 0, size);
    }

    void* _sdlRealloc(void* block, const size_t size) { return platform::realloc(block, size); }

    void _sdlFree(void* block)
    {
        // TODO: Look into how size can be changed
        return platform::free(block, 0);
    }




    bool Platform::startup(const std::string_view appName, const uint32_t x, const uint32_t y, const uint32_t width,
                           const uint32_t height)
    {
        _state = static_cast<PlatformState*>(platform::alloc(sizeof(_state)));
        if (!_state)
        {
            log::error("An error occurred while allocating memory for platform state");
            return false;
        }

        // TODO: Add custom allocator to SDL
        if (!SDL_Init(SDL_INIT_VIDEO))
        {
            log::error("There was an error instantiation SDL!");
            return false;
        }

        _state->window = SDL_CreateWindow(appName.data(), width, height, SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE);
        if (!_state->window)
        {
            log::error("There was an error creating the SDL window!");
            return false;
        }

        // Update the window position
        SDL_SetWindowPosition(_state->window, x, y);


        return true;
    }

    void Platform::shutdown() const
    {
        if (_state && _state->window)
            SDL_DestroyWindow(_state->window);
        SDL_Quit();
    }

    bool Platform::pumpMessages()
    {
        const auto event = static_cast<SDL_Event*>(platform::alloc(sizeof(SDL_Event)));
        while (SDL_PollEvent(event))
        {
            switch (event->type)
            {
                case SDL_EVENT_QUIT:
                    // TODO: Fire closing event
                    return true;
                case SDL_EVENT_WINDOW_RESIZED:
                    // TODO: Handle resizing
                    return true;
                case SDL_EVENT_KEY_DOWN:
                    // TODO: Add input processing
                    return true;
                case SDL_EVENT_KEY_UP:
                    // TODO: Add input processing
                    return true;
                case SDL_EVENT_MOUSE_MOTION:
                    // TODO: Add input processing
                    return true;
                case SDL_EVENT_MOUSE_WHEEL:
                    // TODO: Add input processing
                    return true;
                case SDL_EVENT_MOUSE_BUTTON_DOWN:
                    // TODO: Add input processing
                    return true;
                case SDL_EVENT_MOUSE_BUTTON_UP:
                    // TODO: Add input processing
                    return true;
                default:
                    return false;
            }
        }
        return false;
    }
} // namespace tempest::platform