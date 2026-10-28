#pragma once
/**
 * @file EventListeners.h
 * @author Alan Abraham P Kochumon
 * @date Created on: October 21, 2026
 *
 * @brief Event listener instantiations.
 *
 * @copyright Copyright (c) 2026 Alan Abraham P Kochumon
 */

#include "core/Event.h"

namespace tempest::core
{
    /// Forward declaration
    class Application;

    class ApplicationEventListener: public Listener
    {
    public:
        explicit ApplicationEventListener(Application& app): _app(app) {}
        bool onEvent(EventCode code, void* sender, EventContext context) override;

    private:
        Application& _app;
    };


    class KeyEventListener: public Listener
    {
    public:
        explicit KeyEventListener(Application& app): _app(app) {}
        bool onEvent(EventCode code, void* sender, EventContext context) override;

    private:
        Application& _app;
    };


    class ResizeEventListener: public Listener
    {
    public:
        explicit ResizeEventListener(Application& app): _app(app) {}
        bool onEvent(EventCode code, void* sender, EventContext context) override;

    private:
        Application& _app;
    };
} // namespace tempest::core