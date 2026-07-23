#include "Core/Application.h"

#include <filesystem>
#include <iostream>
#include <string>

int main(int argc, char** argv)
{
    std::string scenePath;

    if (argc > 1)
    {
        scenePath = argv[1];
    }
    else
    {
        const std::filesystem::path executablePath = argv[0];
        const std::string projectName = executablePath.stem().string();
        scenePath = (std::filesystem::path("Scenes") / (projectName + ".scene")).string();
    }

    Engine::Application app;
    if (!app.Initialize(true, scenePath))
    {
        std::cerr << "Player failed to start" << std::endl;
        return 1;
    }

    app.Run();
    app.Shutdown();
    return 0;
}
