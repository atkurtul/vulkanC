#include "device.h"
static device dev;

device* get_device() 
{
    return &dev;
}

void destroy_device()
{
    
}

#ifdef __linux__
#define SURFACE_EXTENSION "VK_KHR_xcb_surface"
#elif _WIN32
#define SURFACE_EXTENSION "VK_KHR_win32_surface"
#endif

const char* extensions[]    = { "VK_EXT_debug_utils", "VK_KHR_surface", SURFACE_EXTENSION };
const char* layers[]        = { "VK_LAYER_KHRONOS_validation" };
const char* device_ext[]    = { "VK_KHR_swapchain" };

uint dbg_cb(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void* pUserData)
{
    printf("%s\n", pCallbackData->pMessage); 
    return VK_FALSE;
}


void create_device()
{
    VkApplicationInfo appinfo = { VK_STRUCTURE_TYPE_APPLICATION_INFO };
    appinfo.engineVersion = VK_MAKE_VERSION(1, 2, 0);
    appinfo.applicationVersion= VK_MAKE_VERSION(1, 2, 0);
    appinfo.apiVersion = VK_MAKE_VERSION(1, 2, 0);

    VkDebugUtilsMessengerCreateInfoEXT debugInfo = { VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT };
    debugInfo.messageSeverity = 
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | 
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | 
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    debugInfo.messageType = 
        VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | 
        VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | 
        VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    debugInfo.pfnUserCallback = dbg_cb;   

    VkInstanceCreateInfo info = { VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO };
    info.pApplicationInfo = &appinfo;
    info.pNext = &debugInfo;
    info.enabledLayerCount = sizeof(layers) / sizeof(char*);
    info.ppEnabledLayerNames = layers;
    info.enabledExtensionCount = sizeof(extensions) / sizeof(char*);
    info.ppEnabledExtensionNames = extensions;
  
    vkCreateInstance(&info, 0, &dev.instance);

    PFN_vkCreateDebugUtilsMessengerEXT proc = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(dev.instance, "vkCreateDebugUtilsMessengerEXT");
    proc(dev.instance, &debugInfo, 0, &dev.messenger);

    {
        uint count;
        vkEnumeratePhysicalDevices(dev.instance, &count, 0);
        VkPhysicalDevice devices[count];
        vkEnumeratePhysicalDevices(dev.instance, &count, devices);
        dev.p_device = devices[0];
    }

    {
        uint count = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(dev.p_device, &count, 0);
        VkQueueFamilyProperties props[count];
        vkGetPhysicalDeviceQueueFamilyProperties(dev.p_device, &count, props);
    }
  
    float prio[3] = { 1, 1, 1 };
    VkDeviceQueueCreateInfo qinfo = { VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO };
    qinfo.queueCount = 3;
    qinfo.pQueuePriorities = prio;

    VkPhysicalDeviceFeatures features = {};
    features.fillModeNonSolid = 1;
    features.sampleRateShading = 1;
    features.vertexPipelineStoresAndAtomics = 1;
    features.samplerAnisotropy = 1;
    VkDeviceCreateInfo deviceInfo = { VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO };
    deviceInfo.queueCreateInfoCount = 1;
    deviceInfo.pQueueCreateInfos = &qinfo;
    deviceInfo.pEnabledFeatures = &features;
    deviceInfo.enabledExtensionCount = sizeof(device_ext) / sizeof(char*);
    deviceInfo.ppEnabledExtensionNames = device_ext;
    deviceInfo.enabledLayerCount = sizeof(layers) / sizeof(char*);
    deviceInfo.ppEnabledLayerNames = layers;
    vkCreateDevice(dev.p_device, &deviceInfo, 0, &dev.device);

    vkGetDeviceQueue(dev.device, 0, 0, &dev.queue[0]);
    vkGetDeviceQueue(dev.device, 0, 1, &dev.queue[1]);
    vkGetDeviceQueue(dev.device, 0, 2, &dev.queue[2]);
}

VkMemoryRequirements buffer_memory_req(VkBuffer buffer)
{
    VkMemoryRequirements req;
    vkGetBufferMemoryRequirements(dev.device, buffer, &req);
    return req;
}

VkMemoryRequirements image_memory_req(VkImage buffer)
{
    VkMemoryRequirements req;
    vkGetImageMemoryRequirements(dev.device, buffer, &req);
    return req; 
}

void vkdbg_printLayers()
{
    uint count;
    vkEnumerateInstanceLayerProperties(&count, 0);
    VkLayerProperties props[count];
    vkEnumerateInstanceLayerProperties(&count, props);
    for(int i = 0; i < count; ++i) printf("%s\n", props[i].layerName);
}

void vkdbg_printExtensions()
{
    uint count;
    vkEnumerateInstanceExtensionProperties(0, &count, 0);
    VkExtensionProperties props[count];
    vkEnumerateInstanceExtensionProperties(0, &count, props);
    for(int i = 0; i < count; ++i) printf("%s\n", props[i].extensionName);
}

void vkdbg_printDeviceExtensions(VkPhysicalDevice device)
{
    uint count;
    vkEnumerateDeviceExtensionProperties(device, 0, &count, 0);
    VkExtensionProperties props[count];
    vkEnumerateDeviceExtensionProperties(device, 0, &count, props);
    for(int i = 0; i < count; ++i) printf("%s\n", props[i].extensionName);
}
