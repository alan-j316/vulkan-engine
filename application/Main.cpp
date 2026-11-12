#include "utils/Logger.h"

#include <Application.h>
#include <print>
#include <string>

void custom_terminate_handler()
{
    tempest::log::error("An unhandled exception occurred!");

    if (std::current_exception())
    {
        try
        {
            throw;
        }
        catch (const std::exception& e)
        {
            tempest::log::error(e.what());
        }
        catch (...)
        {
            tempest::log::error("Unknown exception");
        }
    }
    std::abort();
}

int main()
{
    auto oldHandler = std::set_terminate(custom_terminate_handler);

    const std::string name = "Tempest Application";
    tempest::TempestApp app{ name, 1 };
    if (!app.init())
    {
        tempest::log::error("There was an error initializing the application");
    }
    else
    {
        tempest::log::info("Successfully initalized tempest application");
    }
    app.run();

    std::set_terminate(oldHandler);
}

// Deprecated
// import TempestEngine;
// int main()
// {
//     const std::string appName    = "Tempest Game Engine";
//     const std::string appVersion = "0.0.1";
//     const std::string appId      = "com.tempest.engine";
//
//     try{
//         tempest::TempestEngine vulkanEngine{};
//         vulkanEngine.init(appName, appVersion, appId, 1280, 720);
//         vulkanEngine.run();
//         vulkanEngine.cleanup();
//     } catch (const std::exception& e)
//     {
//         std::println(std::cerr, "Engine Exception\n{}", e.what());
//         return EXIT_FAILURE;
//     }
//     return EXIT_SUCCESS;
// }