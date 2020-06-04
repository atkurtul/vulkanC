#pragma once
#include "swapchain.h"
#include "command_pool.h"
#include "pipeline.h"

typedef struct renderer
{
    cmd_chain                   cmd;
    VkSemaphore*                semaphore;

    VkClearValue*               clear_vals;
    VkRenderPassBeginInfo*      pass_infos;
    VkSubmitInfo*               submit_infos;
    VkPresentInfoKHR*           present_infos;

    VkPipelineLayout            bound;
    uint                        idx;
    
    uint*                       frame_idx; //free later
} renderer;

void create_renderer();

renderer* get_renderer();

void r_begin_frame();

void r_end_frame();

void r_bind_pipeline(pipeline* pipe);

void r_bind_desc_sets(uint first, uint nset, VkDescriptorSet* sets);

void r_bind_vertex_buffers(uint nbuffer, VkBuffer* buffers);

void r_draw(uint ninstance, uint nvertex, uint first_inst, uint first_vertex);

void destroy_renderer();