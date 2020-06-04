#pragma once
#include "pipeline_meta.h"

typedef struct pipeline
{
    VkPipeline      pipe;
    pipeline_meta   meta;

} pipeline;


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
    int blend);
    
void destroy_pipeline(pipeline* pipe);