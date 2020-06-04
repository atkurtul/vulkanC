#include "pipeline.h"


void p_swapchain_recreation_callback(void* ptr)
{
    pipeline* pipe = ptr;
    pipe->meta.pass = get_swapchain()->pass;
    free(pipe->meta.multisampler_info);
    free_viewport_state(pipe->meta.viewport_info);

    pipe->meta.multisampler_info = create_multisampler_state();
    pipe->meta.viewport_info     = create_viewport_state();

    device* dev = get_device();
    vkDestroyPipeline(dev->device, pipe->pipe, 0);
    vkCreateGraphicsPipelines(dev->device, 0, 1, &pipe->meta, 0, &pipe->pipe);
}


pipeline* create_pipeline(
    uint nsets,
    desc_set_layout* layouts,
    const char* vs, 
    const char* ps, 
    const char* vlayout, 
    VkPrimitiveTopology topology, 
    VkPolygonMode mode, 
    int cull, 
    int depth,
    int blend)
{

    pipeline* pipe = calloc(sizeof(pipeline), 1);
    create_pipeline_meta(nsets, layouts, vs, ps, vlayout, topology, mode, cull, depth, blend, &pipe->meta);
    vkCreateGraphicsPipelines(get_device()->device, 0, 1, &pipe->meta, 0, &pipe->pipe);
    push_swapchain_recreation_callback(p_swapchain_recreation_callback, pipe);
    return pipe;
}

void destroy_pipeline(pipeline* pipe)
{
    device* dev = get_device();
    vkDestroyPipeline(dev->device, pipe->pipe, 0);
    vkDestroyPipelineLayout(dev->device, pipe->meta.layout, 0);
    destroy_pipeline_meta(&pipe->meta);
    free(pipe);
}