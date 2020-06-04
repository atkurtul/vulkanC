#include "renderer.h"
#include "swapchain.h"

static renderer ren;

renderer* get_renderer()
{
    return &ren;
}

static VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };

void fill_infos()
{
    swapchain* sc = get_swapchain();
    uint nframes = sc->nframes;
    for (uint i = 0; i < nframes; ++i)
    {
        ren.pass_infos[i].sType                    = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        ren.pass_infos[i].renderPass               = sc->pass;
        ren.pass_infos[i].framebuffer              = sc->frame_buffer[i];
        ren.pass_infos[i].renderArea.extent.width  = sc->width;
        ren.pass_infos[i].renderArea.extent.height = sc->height;
        ren.pass_infos[i].clearValueCount          = 2;
        ren.pass_infos[i].pClearValues             = ren.clear_vals;
        ren.submit_infos[i].sType                  = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        ren.submit_infos[i].waitSemaphoreCount     = 1;
        ren.submit_infos[i].signalSemaphoreCount   = 1;
        ren.submit_infos[i].pWaitDstStageMask      = waitStages;
        ren.submit_infos[i].pWaitSemaphores        = &sc->semaphore[i];
        ren.submit_infos[i].pSignalSemaphores      = &ren.semaphore[i];
        ren.submit_infos[i].commandBufferCount     = 1;
        ren.submit_infos[i].pCommandBuffers        = &ren.cmd.buffers[i];
        ren.present_infos[i].sType                 = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        ren.present_infos[i].waitSemaphoreCount    = 1;
        ren.present_infos[i].pWaitSemaphores       = &ren.semaphore[i];
        ren.present_infos[i].swapchainCount        = 1;
        ren.present_infos[i].pSwapchains           = &sc->chain;
        ren.present_infos[i].pImageIndices         = &ren.frame_idx[i];
        ren.frame_idx[i]                           = i;
    }
}

void prepare_renderer()
{
    ren.clear_vals = calloc(sizeof(VkClearValue), 2);
    VkClearColorValue color = { 0.4, 0.4f, 0.8f, 1.0f };
    VkClearDepthStencilValue depth = { 1.f, 0.f };
    ren.clear_vals[0].color = color;
    ren.clear_vals[1].depthStencil = depth;

    swapchain* sc = get_swapchain();
    uint nframes = sc->nframes;

    ren.frame_idx          = malloc(nframes * 4);
    ren.pass_infos         = calloc(sizeof(VkRenderPassBeginInfo), nframes);
    ren.submit_infos       = calloc(sizeof(VkSubmitInfo), nframes);
    ren.present_infos      = calloc(sizeof(VkPresentInfoKHR), nframes);
    fill_infos(&ren);
}

void create_renderer()
{
    device* dev     = get_device();
    swapchain* sc   = get_swapchain();

    ren.semaphore = calloc(sizeof(VkSemaphore), sc->nframes);
    create_cmd_chain(sc->nframes, &ren.cmd);
    VkSemaphoreCreateInfo info = { VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO };
    for (size_t i = 0; i < sc->nframes; i++) {
        vkCreateSemaphore(dev->device, &info, 0, &ren.semaphore[i]);
    }
    prepare_renderer(&ren);

    push_swapchain_recreation_callback(fill_infos, &ren);
}

void r_begin_frame()
{
    ren.idx = get_next_image();
    cmd_chain_begin(&ren.cmd, ren.idx);
    vkCmdBeginRenderPass(ren.cmd.buffers[ren.idx], ren.pass_infos + ren.idx, VK_SUBPASS_CONTENTS_INLINE);
}

void r_end_frame()
{
    vkCmdEndRenderPass(ren.cmd.buffers[ren.idx]);
    cmd_chain_end(&ren.cmd, RenderQueue, ren.submit_infos + ren.idx);
    vkQueuePresentKHR(get_device()->queue[RenderQueue], ren.present_infos + ren.idx);
}

void r_bind_pipeline(pipeline* pipe)
{
    ren.bound = pipe->meta.layout;
    vkCmdBindPipeline(ren.cmd.buffers[ren.idx], VK_PIPELINE_BIND_POINT_GRAPHICS, pipe->pipe);
}

void r_bind_desc_sets(uint first, uint nset, VkDescriptorSet* sets)
{
    vkCmdBindDescriptorSets(ren.cmd.buffers[ren.idx], VK_PIPELINE_BIND_POINT_GRAPHICS,
        ren.bound, first, nset, sets, 0, 0);
}

void r_bind_vertex_buffers(uint nbuffer, VkBuffer* buffers)
{
    uint64 offsets[10] = {};
    vkCmdBindVertexBuffers(ren.cmd.buffers[ren.idx], 0, nbuffer, buffers, offsets);
}

void r_draw(uint ninstance, uint nvertex, uint first_inst, uint first_vertex)
{
    vkCmdDraw(ren.cmd.buffers[ren.idx], nvertex, ninstance, first_vertex, first_inst);
}

void destroy_renderer()
{
    VkDevice dev = get_device()->device;
    for(int i = 0; i < ren.cmd.nbuffers; ++i)
    {
        vkDestroySemaphore(dev, ren.semaphore[i], 0);
    }
    destroy_cmd_chain(&ren.cmd);
    free(ren.pass_infos);
    free(ren.present_infos);
    free(ren.submit_infos);
    free(ren.semaphore);
    free(ren.frame_idx);
    free(ren.clear_vals);
}