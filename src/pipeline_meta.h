#pragma once
#include "descriptor.h"
#include "swapchain.h"

typedef struct vertex_input_type
{
    VkFormat    format;
    uint        stride;
} vertex_input_type;

typedef struct shader_stages
{
    VkPipelineShaderStageCreateInfo shaders[2];
    const char *vs, *ps;

} shader_stages;

typedef VkPipelineVertexInputStateCreateInfo        vertex_input_layout_desc;

typedef VkPipelineInputAssemblyStateCreateInfo      input_assembler;

typedef VkPipelineViewportStateCreateInfo           viewport_state;

typedef VkPipelineRasterizationStateCreateInfo      rasterizer;

typedef VkPipelineMultisampleStateCreateInfo        multisampler;

typedef VkPipelineDepthStencilStateCreateInfo       depth_stencil_state;

typedef VkPipelineColorBlendStateCreateInfo        color_blend_state;


typedef struct pipeline_meta
{
    uint                        stype;
    void*                       pnext;
    uint                        flags;
    uint                        nstage;
    shader_stages*              shader_info;
    vertex_input_layout_desc*   vertex_info;
    input_assembler*            assembler_info;
    void*                       tesselation;
    viewport_state*             viewport_info;
    rasterizer*                 rasterizer_info;
    multisampler*               multisampler_info;
    depth_stencil_state*        depth_stencil_info;
    color_blend_state*          blend_info;
    void*                       dynamic_state;
    VkPipelineLayout            layout;
    VkRenderPass                pass;
    uint                        subpass;
    VkPipeline                  base;
    int                         idx;

} pipeline_meta;


void create_pipeline_meta(
    uint nsets,
    desc_set_layout* layouts,
    const char* vs, 
    const char* ps, 
    const char* vlayout, 
    VkPrimitiveTopology topology, 
    VkPolygonMode mode, 
    int cull, 
    int depth,
    int blend,
    pipeline_meta* meta);

void destroy_pipeline_meta(pipeline_meta* meta);

VkShaderModule compile_shader(const char* path);

viewport_state* create_viewport_state();

void free_viewport_state(viewport_state* state);

multisampler* create_multisampler_state();