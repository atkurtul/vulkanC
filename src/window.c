#include "window.h"

static window win;

window* get_window()
{
    return &win;
}

VkSurfaceKHR create_surface()
{
    device* dev = get_device();

    VkSurfaceKHR surface;
    glfwCreateWindowSurface(dev->instance, win.window, 0, &surface);

    uint supported;
    VkSurfaceCapabilitiesKHR cap;
    vkGetPhysicalDeviceSurfaceSupportKHR(dev->p_device, 0, surface, &supported);
    
    uint count;
    vkGetPhysicalDeviceSurfacePresentModesKHR(dev->p_device, surface, &count, 0);
    VkPresentModeKHR modes[count];
    vkGetPhysicalDeviceSurfacePresentModesKHR(dev->p_device, surface, &count, modes);
    return surface;
}

void create_window(int x, int y, int fs)
{
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    win.window = glfwCreateWindow(x, y, "Vulkan", fs ? glfwGetPrimaryMonitor() : 0, 0);
    glfwGetWindowSize(win.window, &win.x, &win.y);
    win.inv_size[0] = 1.f / (float)win.x;
    win.inv_size[1] = 1.f / (float)win.y;
    win.surface = create_surface();
}


void window_resize_callback(GLFWwindow* window,int x,int y)
{
    win.x = x;
    win.y = y;
    win.inv_size[0] = 1.f / (float)x;
    win.inv_size[1] = 1.f / (float)y;
}

int poll_window()
{
    glfwPollEvents();
    glfwSetWindowSizeCallback(win.window, window_resize_callback);
    if(glfwGetKey(win.window, GLFW_KEY_ESCAPE)) return 0;
    return !glfwWindowShouldClose(win.window);
}