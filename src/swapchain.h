#pragma once
#include "device.h"
#include "window.h"

typedef struct image_buffer
{
    VkImage            image;
    VkImageView        view;
    VkDeviceMemory     mem;
} image_buffer;

typedef void (*fn_callback)();

typedef struct callback
{
    fn_callback fn;
    void*       ptr;
} callback;

typedef struct swapchain {
    uint swapidx;
    uint nframes;
    uint msaa;
    uint width, height;
    VkSwapchainKHR      chain;
    VkRenderPass        pass;
    VkImage*            images;
    VkImageView*        views;

    image_buffer        depth_buffer;
    image_buffer        color_buffer;

    VkFramebuffer*      frame_buffer;
    VkSemaphore*        semaphore;

    callback*   callbacks;
    uint        ncallback;
} swapchain;


void        create_swapchain(uint nframes, uint MSAA);

swapchain*  get_swapchain();

uint        get_next_image();

void        recreate_swapchain();

void        push_swapchain_recreation_callback(fn_callback fn, void* ptr);

void        destroy_swapchain();

image_buffer create_image_buffer(VkSampleCountFlagBits MSAA, int x, int y, VkFormat format, VkImageUsageFlags usage, VkImageAspectFlags aspect, uint mip);

void free_image_buffer(image_buffer buff);