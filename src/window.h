#pragma once
#include "device.h"
#include "vmath.h"
typedef struct window 
{
    GLFWwindow* window;
    VkSurfaceKHR surface;
    int x, y;

    vec2 inv_size;
} window;

void create_window(int x, int y, int fs);

int poll_window();

window* get_window();

