/**
 * @file Main.cpp
 * @author Alan Abraham P Kochumon
 * @date Created on: October 14, 2026
 *
 * @brief Game application entry point.
 *
 * @copyright Copyright (c) 2026 Alan Abraham P Kochumon
 */

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
    auto oldHandler = std::set_terminate(custom_terminate_handler);
    tempest::TempestEngine engine{};

    if (!engine.startup())
    {
        std::println("There was an error starting the engine");
        return EXIT_FAILURE;
    }

    engine.run();

    engine.shutdown();
    std::set_terminate(oldHandler);
    return EXIT_SUCCESS;
}