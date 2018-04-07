#include <SDL.h>
#include <vulkan/vulkan.h>
#include <SDL_vulkan.h>

#include <iostream>
#include <stdexcept>
#include <functional>
#include <cstdlib>
#include <vector>

#define WIDTH 800
#define HEIGHT 600

#define MAJOR_VERSION 1
#define MINOR_VERSION 0
#define POINT_VERSION 0

class HelloTriangleApplication
{

    SDL_Window* window;
    VkInstance instance;

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
        createInstance();
    }

    void mainLoop()
    {
        //Loop forever
        while(true)
        {
            //Pop events of SDL event queue
            SDL_Event event;
            while(SDL_PollEvent(&event))
            {
                //Exit by clicking X in window or pressing ESC
                if(event.type == SDL_QUIT || (event.type == SDL_KEYDOWN && event.key.keysym.scancode == SDL_SCANCODE_ESCAPE))
                    return;
            }
        }
    }

    void cleanup()
    {
        vkDestroyInstance(instance, NULL);

        SDL_Vulkan_UnloadLibrary();
        SDL_DestroyWindow(window);
        SDL_Quit();
    }

    void createInstance()
    {
        //Create application info struct
        VkApplicationInfo appInfo = {};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = "Vulkan SDL";
        appInfo.applicationVersion = VK_MAKE_VERSION(MAJOR_VERSION, MINOR_VERSION, POINT_VERSION);
        appInfo.pEngineName = "No Engine";
        appInfo.engineVersion = VK_MAKE_VERSION(MAJOR_VERSION, MINOR_VERSION, POINT_VERSION);
        appInfo.apiVersion = VK_API_VERSION_1_1;

        //Get required extensions for SDL (https://gist.github.com/rcgordon/ad23f873393423e1f1069502b92ad035)
        unsigned int count = 0;
        const char **names = NULL;
        SDL_Vulkan_GetInstanceExtensions(window, &count, NULL);
        names = new const char *[count];
        SDL_Vulkan_GetInstanceExtensions(window, &count, names);

        //Show required extensions for SDL
        std::cout << "Required Extensions:" << std::endl;
        for(unsigned int i = 0; i < count; i++)
        {
            std::cout << names[i] << std::endl;
        }

        //Show available extensions
        uint32_t extensionCount = 0;
        vkEnumerateInstanceExtensionProperties(NULL, &extensionCount, NULL);
        std::vector<VkExtensionProperties> extensions(extensionCount);
        vkEnumerateInstanceExtensionProperties(NULL, &extensionCount, extensions.data());
        std::cout << "Available extensions:" << std::endl;
        for(const auto& extension : extensions)
        {
            std::cout << extension.extensionName << std::endl;
        }

        //Create application instance info struct
        VkInstanceCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pApplicationInfo = &appInfo;
        createInfo.enabledExtensionCount = count;
        createInfo.ppEnabledExtensionNames = names;
        createInfo.enabledLayerCount = 0;
        if(vkCreateInstance(&createInfo, NULL, &instance) != VK_SUCCESS)
        {
            std::cout << "Error creating Vulkan instance" << std::endl;
            exit(1);
        }

        //Cleanup allocated memory
        delete[] names;
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
