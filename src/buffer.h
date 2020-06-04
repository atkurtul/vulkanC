#pragma once
#include "device.h"
#include "command_pool.h"

typedef struct buffer 
{
    VkBuffer            buff;
    VkDeviceMemory      mem;
} buffer;

typedef struct mapped_buffer
{
    VkBuffer            buff;
    VkDeviceMemory      mem;
    void*               ptr;
    uint64              size;
} mapped_buffer;


mapped_buffer create_mapped_buffer(buffer_usage usage, uint64 size);

buffer create_buffer(buffer_usage usage, const void* src, uint64 size);

void free_buffer(void* buff);