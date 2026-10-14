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

int main()
{
    tempest::TempestEngine engine{};

    if (!engine.startup())
    {
        std::println("There was an error starting the engine");
        return EXIT_FAILURE;
    }

    engine.run();

    engine.shutdown();
    return EXIT_SUCCESS;
}