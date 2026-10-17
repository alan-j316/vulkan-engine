/**
 * @file Main.cpp
 * @author Alan Abraham P Kochumon
 * @date Created on: October 14, 2026
 *
 * @brief Game application entry point.
 *
 * @copyright Copyright (c) 2026 Alan Abraham P Kochumon
 */

#include "Application.h"

#include <Tempest.h>
#include <cstdlib>
#include <print>


void custom_terminate_handler()
{
    std::println("An unhandled exception occurred!");

    if (std::current_exception())
    {
        try
        {
            throw;
        }
        catch (const std::exception& e)
        {
            std::println("{}", e.what());
        }
        catch (...)
        {
            std::println("Unknown exception");
        }
    }
    std::abort();
}


int main()
{
    std::set_terminate(custom_terminate_handler);


    tempest::AppConfig config{ .startPosX = 100, .startPosY = 100, .width = 1280, .height = 720, .name = "Tempest" };
    tempest::core::Application application;
    application.create(config);
    application.run();
    // auto oldHandler = std::set_terminate(custom_terminate_handler);
    // tempest::TempestEngine engine{};
    //
    // if (!engine.startup())
    // {
    //     std::println("There was an error starting the engine");
    //     return EXIT_FAILURE;
    // }
    //
    // engine.run();
    //
    // engine.shutdown();
    // std::set_terminate(oldHandler);
    // return EXIT_SUCCESS;
}