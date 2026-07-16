#include "Core/Application.h"

#include <iostream>

int main()
{
    Engine::Application app;

    if (!app.Initialize())
    {
        std::cerr << "Engine failed to start" << std::endl;
        return 1;
    }

    app.Run();
    app.Shutdown();
    return 0;
}
