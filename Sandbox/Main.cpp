#include "Core/Application.h"

#include <iostream>
#include <string>

int main(int argc, char** argv)
{
    Engine::Application app;
    bool playOnly = false;
    std::string scenePath;

    for (int index = 1; index < argc; index++)
    {
        const std::string argument = argv[index];
        if (argument == "--play")
        {
            playOnly = true;
            if (index + 1 < argc)
            {
                scenePath = argv[++index];
            }
        }
    }

    if (!app.Initialize(playOnly, scenePath))
    {
        std::cerr << "Engine failed to start" << std::endl;
        return 1;
    }

    app.Run();
    app.Shutdown();
    return 0;
}
