#include "command_pool.h"

VkCommandPool pool;

void submit_cmd(cmd_buffer* cmd, queueID q)
{
    device* dev = get_device();
    VkSubmitInfo info = { VK_STRUCTURE_TYPE_SUBMIT_INFO };
    info.commandBufferCount = 1;
    info.pCommandBuffers = &cmd->buffer;
    vkEndCommandBuffer(cmd->buffer);
    vkQueueSubmit(dev->queue[q], 1, &info, cmd->fence);
    vkWaitForFences(dev->device, 1, &cmd->fence, 1, -1);
    vkDestroyFence(dev->device, cmd->fence, 0);
    vkFreeCommandBuffers(dev->device, pool, 1, &cmd->buffer);
    free(cmd);
}

void init_cmd_pools()
{
    VkCommandPoolCreateInfo info = { VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO };
    info.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT | VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    info.queueFamilyIndex = 0;
    vkCreateCommandPool(get_device()->device, &info, 0, &pool);
}

cmd_buffer* request_cmd()
{
    device* dev = get_device();
    cmd_buffer* buffer = malloc(sizeof(cmd_buffer));

    VkFenceCreateInfo fenceInfo = { VK_STRUCTURE_TYPE_FENCE_CREATE_INFO };
    vkCreateFence(dev->device, &fenceInfo, 0, &buffer->fence);

    VkCommandBufferAllocateInfo allocInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO, 0, pool, 
        VK_COMMAND_BUFFER_LEVEL_PRIMARY, 1 };
    vkAllocateCommandBuffers(dev->device, &allocInfo, &buffer->buffer);
    VkCommandBufferBeginInfo beginInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO , 0,
    VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT };
    vkBeginCommandBuffer(buffer->buffer, &beginInfo);

    return buffer;
}

void create_cmd_chain(uint nbuffers, cmd_chain* cmd)
{
    device* dev = get_device();

    cmd->nbuffers   = nbuffers;
    cmd->buffers    = malloc(sizeof(VkCommandBuffer) * nbuffers);
    cmd->fences     = malloc(sizeof(VkFence) * nbuffers);

    VkCommandPoolCreateInfo poolInfo = { VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO };
    poolInfo.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT | VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    vkCreateCommandPool(dev->device, &poolInfo, 0, &cmd->pool);

    VkFenceCreateInfo fenceInfo = { VK_STRUCTURE_TYPE_FENCE_CREATE_INFO, 0, VK_FENCE_CREATE_SIGNALED_BIT };
    for (uint i = 0; i < nbuffers; i++) {
        vkCreateFence(dev->device, &fenceInfo, 0, &cmd->fences[i]);
    }

    VkCommandBufferAllocateInfo allocInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO };
    allocInfo.commandPool = cmd->pool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = nbuffers;
    vkAllocateCommandBuffers(dev->device, &allocInfo, cmd->buffers);
}

VkCommandBuffer cmd_cbuffer(cmd_chain* cmd)
{
    return cmd->buffers[cmd->idx];
}

void cmd_chain_begin(cmd_chain* cmd, uint idx)
{
    device* dev = get_device();

    cmd->idx = idx;
    vkWaitForFences(dev->device, 1, cmd->fences + idx, 1, -1);
    vkResetFences(dev->device, 1, &cmd->fences[cmd->idx]);
    vkResetCommandBuffer(cmd->buffers[cmd->idx], VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT);
    VkCommandBufferBeginInfo beginInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO , 0, VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT };
    vkBeginCommandBuffer(cmd->buffers[cmd->idx], &beginInfo);
}


void cmd_chain_end(cmd_chain* cmd, queueID q, VkSubmitInfo* info)
{
    vkEndCommandBuffer(cmd->buffers[cmd->idx]);
    vkQueueSubmit(get_device()->queue[q], 1, info, cmd->fences[cmd->idx]);
}

void destroy_cmd_chain(cmd_chain* cmd)
{
    VkDevice dev = get_device()->device;
    vkFreeCommandBuffers(dev, cmd->pool, cmd->nbuffers, cmd->buffers);
    vkDestroyCommandPool(dev, cmd->pool, 0);
    for(int i = 0; i < cmd->nbuffers; ++i)
    {
        vkDestroyFence(dev, cmd->fences[i], 0);
    }
    free(cmd->buffers);
    free(cmd->fences);
}