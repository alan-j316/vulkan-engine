#pragma once
/**
 * @file RendererEventQueue.h
 * @author Alan Abraham P Kochumon
 * @date Created on: November 05, 2026
 *
 * @brief Handles renderer events to and from other systems.
 *
 * @copyright Copyright (c) 2026 Alan Abraham P Kochumon
 */

#include "core/assets/Asset.h"

#include <cstddef>
#include <cstdint>
#include <vector>

namespace tempest::renderer
{

    static constexpr size_t RenderQueueMaxEvents = 128;


    /**
     * A event that requests data upload to the renderer.
     */
    struct RenderUploadEvent
    {
        Vector<core::Vertex> vertices{ makeVector<core::Vertex>() };
        Vector<uint32_t> indices{ makeVector<uint32_t>() };
    };


    class RendererEventQueue
    {
    public:
        RendererEventQueue() { _uploadEvents.reserve(RenderQueueMaxEvents); }

        template <typename Event>
        void push(const Event& event) noexcept
        { _uploadEvents.push_back(event); }

        template <typename Event>
        bool poll(Event& event) noexcept
        {
            if (_uploadEvents.empty())
                return false;

            event = _uploadEvents.back();
            _uploadEvents.pop_back();
            return true;
        }

    private:
        // TODO: Refactor to circular queue
        Vector<RenderUploadEvent> _uploadEvents{ makeVector<RenderUploadEvent>() };
        // std::atomic_int front;
        // std::atomic_int back;
    };
} // namespace tempest::renderer
