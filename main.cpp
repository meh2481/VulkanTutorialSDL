#include <SDL.h>
#include <vulkan/vulkan.h>

#include <iostream>
#include <stdexcept>
#include <functional>
#include <cstdlib>

class HelloTriangleApplication
{
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

    }

    void initVulkan()
    {

    }

    void mainLoop()
    {

    }

    void cleanup()
    {

    }
};

#ifdef _WIN32
int SDL_main(int argc, char* argv[])
#else
int main(int argc, char** argv)
#endif
{
    HelloTriangleApplication app;

    try
    {
        app.run();
    }
    catch(const std::runtime_error& e)
    {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
