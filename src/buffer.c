#include "buffer.h"

VkBuffer create_buffer_1(buffer_usage usage, uint64 size)
{
    VkBufferCreateInfo info = { VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
    info.usage        = usage;
    info.sharingMode  = VK_SHARING_MODE_EXCLUSIVE;
    info.size         = size;

    VkBuffer buffer;

    vkCreateBuffer(get_device()->device, &info, 0, &buffer);
    return buffer;
}

VkDeviceMemory allocate_memory(VkBuffer buffer, uint memType)
{
    VkMemoryAllocateInfo allocInfo = { VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO };
    allocInfo.allocationSize = buffer_memory_req(buffer).size;
    allocInfo.memoryTypeIndex = memType;

    VkDeviceMemory mem;

    device* dev = get_device();
    vkAllocateMemory(dev->device, &allocInfo, 0, &mem);
    vkBindBufferMemory(dev->device, buffer, mem, 0);
    return mem;
}

mapped_buffer create_mapped_buffer(buffer_usage usage, uint64 size)
{
    mapped_buffer buff;
    buff.buff   = create_buffer_1(usage, size);
    buff.mem    = allocate_memory(buff.buff, SHARED_MEMORY);
    buff.size   = size;
    vkMapMemory(get_device()->device, buff.mem, 0, size, 0, &buff.ptr);
    return buff;
}

buffer create_buffer(buffer_usage usage, const void* src, uint64 size)
{
    mapped_buffer staging = create_mapped_buffer(usage_src, size);
    memcpy(staging.ptr, src, size);
    vkUnmapMemory(get_device()->device, staging.mem);

    buffer buff;
    buff.buff   = create_buffer_1(usage_dst | usage, size);
    buff.mem    = allocate_memory(buff.buff, LOCAL_MEMORY);
    
    cmd_buffer* cmd = request_cmd();
    VkBufferCopy copyRegion = { 0, 0, size };
    vkCmdCopyBuffer(cmd->buffer, staging.buff, buff.buff, 1, &copyRegion);
    submit_cmd(cmd, CopyQueue);
    free_buffer(&staging);
    return buff;
}

void free_buffer(void* buff)
{
    device* dev = get_device();
    buffer* b = buff;
    vkDestroyBuffer(dev->device, b->buff, 0);
    vkFreeMemory(dev->device, b->mem, 0);
}