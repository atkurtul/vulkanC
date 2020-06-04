#pragma once
#include "swapchain.h"

typedef struct texture
{
    image_buffer    buffer;
    VkSampler       sampler;
} texture;

texture create_texture(const char* path);