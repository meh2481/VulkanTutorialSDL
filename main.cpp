#include <SDL.h>
#include <vulkan/vulkan.h>
#include <SDL_vulkan.h>

#include <iostream>
#include <stdexcept>
#include <functional>
#include <cstdlib>

#define WIDTH 800
#define HEIGHT 600

class HelloTriangleApplication
{

    SDL_Window* window;

public:
    void run()
    {
        initWindow();
        initVulkan();
        mainLoop();
        cleanup();
    }

private:

    void initWindow()
    {
        SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);
        if(SDL_Vulkan_LoadLibrary(NULL) == -1)
        {
            std::cout << "Error loading vulkan" << std::endl;
            exit(1);
        }
        atexit(SDL_Quit);

        window = SDL_CreateWindow("Vulkan SDL",
            SDL_WINDOWPOS_UNDEFINED,
            SDL_WINDOWPOS_UNDEFINED,
            WIDTH,
            HEIGHT,
            SDL_WINDOW_SHOWN | SDL_WINDOW_VULKAN);

        if(window == NULL)
        {
            std::cout << "Couldn't set video mode: " << SDL_GetError() << std::endl;
            exit(1);
        }
    }

    void initVulkan()
    {

    }

    void mainLoop()
    {
        while(true)
        {
            SDL_Event event;
            while(SDL_PollEvent(&event))
            {
                if(event.type == SDL_QUIT)
                    return;
            }
        }
    }

    void cleanup()
    {
        SDL_Vulkan_UnloadLibrary();
        SDL_DestroyWindow(window);
        SDL_Quit();
    }
};

#ifdef _WIN32
int SDL_main(int argc, char* argv[])
#else
int main(int argc, char** argv)
#endif
{
    HelloTriangleApplication app;

    app.run();

    return EXIT_SUCCESS;
}
