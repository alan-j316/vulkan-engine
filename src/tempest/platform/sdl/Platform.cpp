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
#include "renderer/vulkan/VulkanPlatform.h"

#include "core/Event.h"
#include "core/Input.h"
#include "core/Logger.h"
#include "core/MemorySubsystem.h"
#include "platform/Memory.h"
#include "renderer/vulkan/VulkanTypes.inl" // TODO: Update by separating the logic into different compilation unit

#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>

namespace tempest::platform
{
    struct PlatformState
    {
        SDL_Window* window{ nullptr };
    };


    /// CUSTOM MEMORY FUNCTION PASS THROUGH FOR SDL
    namespace
    {
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

    } // namespace



    bool Platform::startup(const std::string_view appName, const uint32_t x, const uint32_t y, const uint32_t width,
                           const uint32_t height)
    {
        _state = static_cast<PlatformState*>(memory::alloc(sizeof(_state), memory::MemoryTag::MEMORY_TAG_APPLICATION));

        // TODO: Replace SDL's mem functions with custom functions
        //
        // if (!SDL_SetMemoryFunctions(_sdlMalloc, _sdlCalloc, _sdlRealloc, _sdlFree))
        // {
        //     log::error("An error occurred while switching SDL's memory functions");
        //     return false;
        // }

        if (!_state)
        {
            log::error("An error occurred while allocating memory for platform state");
            return false;
        }

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

        // Free the used state memory
        memory::free(_state, sizeof(PlatformState), memory::MemoryTag::MEMORY_TAG_APPLICATION);
    }


    bool Platform::pumpMessages()
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
                case SDL_EVENT_QUIT:
                    core::fireEvent(static_cast<core::EventCode>(core::SystemEventCode::APPLICATION_QUIT), nullptr, {});
                    return false;
                case SDL_EVENT_WINDOW_RESIZED:
                    // TODO: Handle resizing
                    return true;
                case SDL_EVENT_KEY_DOWN:
                    core::input::processKey(static_cast<core::Keycode>(event.key.key), true);
                    return true;
                case SDL_EVENT_KEY_UP:
                    core::input::processKey(static_cast<core::Keycode>(event.key.key), false);
                    return true;
                case SDL_EVENT_MOUSE_MOTION: {
                    float x, y;
                    SDL_GetMouseState(&x, &y);
                    const core::MousePosition pos{ .x = static_cast<uint16_t>(x), .y = static_cast<uint16_t>(y) };
                    core::input::processMouseMove(pos);
                }
                    return true;
                case SDL_EVENT_MOUSE_WHEEL: {
                    const auto scroll = event.wheel.y;
                    if (std::abs(scroll) < 1e5)
                        return false;
                    core::input::processMouseScroll(scroll > 0 ? 1 : -1);
                }
                    return true;
                case SDL_EVENT_MOUSE_BUTTON_DOWN: {
                    const auto button = static_cast<core::Button>(event.button.button);
                    core::input::processButton(button, true);
                }
                    return true;
                case SDL_EVENT_MOUSE_BUTTON_UP: {
                    const auto button = static_cast<core::Button>(event.button.button);
                    core::input::processButton(button, false);
                }
                    return true;
                default:
                    return false;
            }
        }
        return true;
    }


    Vector<const char*> getRequiredExtensions()
    {
        uint32_t extensionsCount;
        const auto ext = SDL_Vulkan_GetInstanceExtensions(&extensionsCount);
        // TODO: Refactor to PMR
        return Vector<const char*>(ext, ext + extensionsCount);
    }


    bool createVulkanSurface(const PlatformState& platformState, renderer::VulkanContext& context)
    {
        VkSurfaceKHR surface;
        if (!SDL_Vulkan_CreateSurface(platformState.window, *context.instance, nullptr, &surface))
        {
            return false;
        }
        context.surface = vk::raii::SurfaceKHR(context.instance, surface);
        return context.surface != nullptr;
    }
} // namespace tempest::platform