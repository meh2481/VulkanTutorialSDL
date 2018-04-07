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
#define APPLICATION_NAME "Vulkan SDL"

#define MAJOR_VERSION 1
#define MINOR_VERSION 0
#define POINT_VERSION 0

#define VULKAN_API_VERSION VK_API_VERSION_1_1

const std::vector<const char*> validationLayers = {
    "VK_LAYER_LUNARG_standard_validation"
};

#ifndef NDEBUG
#define ENABLE_VALIDATION_LAYERS

VkResult createDebugReportCallbackEXT(VkInstance instance, const VkDebugReportCallbackCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugReportCallbackEXT* pCallback)
{
    auto func = (PFN_vkCreateDebugReportCallbackEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugReportCallbackEXT");
    if(func != NULL)
        return func(instance, pCreateInfo, pAllocator, pCallback);
    else
        return VK_ERROR_EXTENSION_NOT_PRESENT;
}

void destroyDebugReportCallbackEXT(VkInstance instance, VkDebugReportCallbackEXT callback, const VkAllocationCallbacks* pAllocator)
{
    auto func = (PFN_vkDestroyDebugReportCallbackEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugReportCallbackEXT");
    if(func != NULL)
        func(instance, callback, pAllocator);
}
#endif

//#define PAUSE_HACK
#ifdef PAUSE_HACK
void pause()
{
    system("pause");
}
#endif

class HelloTriangleApplication
{

private:
    //Member variables
    SDL_Window* window;
    VkInstance instance;
#ifdef ENABLE_VALIDATION_LAYERS
    VkDebugReportCallbackEXT callback;
#endif // ENABLE_VALIDATION_LAYERS
    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;   //Implicitly destroyed when VkInstance is

public:
    void run()
    {
#ifdef PAUSE_HACK
        atexit(pause);
#endif
        initWindow();
        initVulkan();
        mainLoop();
        cleanup();
    }

private:

#ifdef ENABLE_VALIDATION_LAYERS
    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
        VkDebugReportFlagsEXT flags,
        VkDebugReportObjectTypeEXT objType,
        uint64_t obj,
        size_t location,
        int32_t code,
        const char* layerPrefix,
        const char* msg,
        void* userData)
    {

        std::cout << "Validation layer: " << msg << std::endl;

        return VK_FALSE;
    }

    void setupDebugCallback()
    {
        VkDebugReportCallbackCreateInfoEXT createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
        createInfo.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT;
        createInfo.pfnCallback = debugCallback;

        if(createDebugReportCallbackEXT(instance, &createInfo, NULL, &callback) != VK_SUCCESS)
        {
            std::cout << "failed to set up debug callback!" << std::endl;
            exit(1);
        }
    }
#endif

    void initWindow()
    {
        SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);
        if(SDL_Vulkan_LoadLibrary(NULL) == -1)
        {
            std::cout << "Error loading vulkan" << std::endl;
            exit(1);
        }
        atexit(SDL_Quit);

        window = SDL_CreateWindow(APPLICATION_NAME,
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
#ifdef ENABLE_VALIDATION_LAYERS
        setupDebugCallback();
#endif // ENABLE_VALIDATION_LAYERS
        pickPhysicalDevice();
    }

    void pickPhysicalDevice()
    {
        uint32_t deviceCount = 0;
        vkEnumeratePhysicalDevices(instance, &deviceCount, NULL);
        if(deviceCount == 0)
        {
            std::cout << "No physical device has Vulkan support" << std::endl;
            exit(1);
        }
        std::cout << "Found " << deviceCount << " physical devices" << std::endl;
        std::vector<VkPhysicalDevice> devices(deviceCount);
        vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

        //Find suitable device
        for(const auto& device : devices)
        {
            if(isDeviceSuitable(device))
            {
                physicalDevice = device;
                break;
            }
        }

        if(physicalDevice == VK_NULL_HANDLE)
        {
            std::cout << "Failed to find a suitable GPU" << std::endl;
            exit(1);
        }
    }

    bool isDeviceSuitable(VkPhysicalDevice device)
    {
        VkPhysicalDeviceProperties deviceProperties;
        vkGetPhysicalDeviceProperties(device, &deviceProperties);
        std::cout << "Device name: " << deviceProperties.deviceName << std::endl;

        VkPhysicalDeviceFeatures deviceFeatures;
        vkGetPhysicalDeviceFeatures(device, &deviceFeatures);


        return deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU && deviceFeatures.geometryShader;
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
#ifdef ENABLE_VALIDATION_LAYERS
        destroyDebugReportCallbackEXT(instance, callback, NULL);
#endif // ENABLE_VALIDATION_LAYERS
        vkDestroyInstance(instance, NULL);

        SDL_Vulkan_UnloadLibrary();
        SDL_DestroyWindow(window);
        SDL_Quit();
    }

    void createInstance()
    {
#ifdef ENABLE_VALIDATION_LAYERS
        if(!checkValidationLayerSupport())
        {
            std::cout << "No validation layers supported" << std::endl;
            exit(1);
        }
#endif

        //Create application info struct
        VkApplicationInfo appInfo = {};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = APPLICATION_NAME;
        appInfo.applicationVersion = VK_MAKE_VERSION(MAJOR_VERSION, MINOR_VERSION, POINT_VERSION);
        appInfo.pEngineName = "No Engine";
        appInfo.engineVersion = VK_MAKE_VERSION(MAJOR_VERSION, MINOR_VERSION, POINT_VERSION);
        appInfo.apiVersion = VULKAN_API_VERSION;

        //Get required extensions
        unsigned int count = 0;
        SDL_Vulkan_GetInstanceExtensions(window, &count, NULL);
        const char **names = new const char *[count];
        std::vector<const char*> requiredExtensions = getRequiredExtensions(names, count);

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
        createInfo.enabledExtensionCount = requiredExtensions.size();
        createInfo.ppEnabledExtensionNames = requiredExtensions.data();
#ifdef ENABLE_VALIDATION_LAYERS
        createInfo.enabledLayerCount = validationLayers.size();
        createInfo.ppEnabledLayerNames = validationLayers.data();
#else
        createInfo.enabledLayerCount = 0;
#endif
        if(vkCreateInstance(&createInfo, NULL, &instance) != VK_SUCCESS)
        {
            std::cout << "Error creating Vulkan instance" << std::endl;
            exit(1);
        }

        //Cleanup allocated memory
        delete[] names;
    }

    std::vector<const char*> getRequiredExtensions(const char **names, unsigned int count)
    {
        //Get required extensions for SDL (https://gist.github.com/rcgordon/ad23f873393423e1f1069502b92ad035)
        SDL_Vulkan_GetInstanceExtensions(window, &count, names);

        //Show required extensions for SDL
        std::cout << "Required Extensions:" << std::endl;
        for(unsigned int i = 0; i < count; i++)
        {
            std::cout << names[i] << std::endl;
        }

        std::vector<const char*> extensions(names, names + count);

#ifdef ENABLE_VALIDATION_LAYERS
        extensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
#endif

        return extensions;
    }

#ifdef ENABLE_VALIDATION_LAYERS
    bool checkValidationLayerSupport()
    {
        uint32_t layerCount;
        vkEnumerateInstanceLayerProperties(&layerCount, NULL);

        std::vector<VkLayerProperties> availableLayers(layerCount);
        vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

        for(const char* layerName : validationLayers)
        {
            bool layerFound = false;

            for(const auto& layerProperties : availableLayers)
            {
                if(strcmp(layerName, layerProperties.layerName) == 0)
                {
                    layerFound = true;
                    break;
                }
            }

            if(!layerFound)
                return false;
        }

        return true;
    }
#endif

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
