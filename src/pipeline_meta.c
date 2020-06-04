#include "pipeline_meta.h"
#include "string.h"
#include "unistd.h"

VkShaderModule compile_shader(const char* path)
{
    char cwd[256] = { };   
    char* dump = getcwd(cwd, 256);
    strcat(cwd, "/shaders/");
    strcat(cwd, path);
    char cmd[512] = { };
    strcat(cmd, "glslc -c ");
    strcat(cmd, cwd);
    strcat(cmd, " -o ");

    strcat(cwd, "out.spv");
    strcat(cmd, cwd);  
    int dd = system(cmd);
    FILE* code = fopen(cwd, "rb");
    uint res = fseek(code, 0, SEEK_END);
    uint size = ftell(code);
    rewind(code);

    char* byteCode = malloc(size);
    uint64 read = fread(byteCode, 1, size, code);
    
    VkShaderModuleCreateInfo info = { VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO };
    info.pCode = (uint*)byteCode;
    info.codeSize = size;

    VkShaderModule mod;
    vkCreateShaderModule(get_device()->device, &info, 0, &mod);
    free(byteCode);
    fclose(code);
    
    return mod;
}

vertex_input_type get_vertex_input_type(char stride, char type)
{
    vertex_input_type vtype;
    vtype.stride = stride - '0';
    switch(type)
    {
        case 'u': vtype.format = 95; break;
        case 'i': vtype.format = 96; break;
        case 'f': vtype.format = 97; break;
    }
    vtype.format += vtype.stride * 3; 
    vtype.stride *= 4;
    return vtype;
}


vertex_input_layout_desc* create_vertex_input_layout_desc(const char* cdesc)
{
    uint nattribs = strlen(cdesc) / 2;

    vertex_input_layout_desc* desc = calloc(sizeof(vertex_input_layout_desc), 1);
    VkVertexInputAttributeDescription* attribs = calloc(sizeof(VkVertexInputAttributeDescription), nattribs);
    VkVertexInputBindingDescription* binding = calloc(sizeof(VkVertexInputBindingDescription), 1);

    desc->sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    desc->pVertexAttributeDescriptions = attribs;
    desc->pVertexBindingDescriptions = binding;
    desc->vertexBindingDescriptionCount = 1;
    desc->vertexAttributeDescriptionCount = nattribs;

    for(int i = 0; i < nattribs; ++i)
    {
        vertex_input_type vtype = get_vertex_input_type(cdesc[i * 2], cdesc[i * 2 + 1]);
        attribs[i].binding = 0;
        attribs[i].format = vtype.format;
        attribs[i].offset = binding->stride;
        attribs[i].location = i;
        binding->stride += vtype.stride;
    }

    return desc;
}

void free_vertex_input_layout_desc(vertex_input_layout_desc* desc)
{
    free((void*)desc->pVertexBindingDescriptions);
    free((void*)desc->pVertexAttributeDescriptions);
    free(desc);
}

shader_stages* create_shader_stage_info(const char* vs, const char* ps)
{
    shader_stages* stages = calloc(sizeof(shader_stages), 1);
    stages->shaders[0].sType = stages->shaders[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    stages->shaders[0].pName = stages->shaders[1].pName = "main";

    stages->shaders[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
    stages->shaders[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;

    stages->shaders[0].mod = compile_shader(vs);
    stages->shaders[1].mod = compile_shader(ps);
    
    stages->vs = vs;
    stages->ps = ps;

    return stages;
}   

void free_shader_stage_info(shader_stages* stages)
{
    VkDevice dev = get_device()->device;
    vkDestroyShaderModule(dev, stages->shaders[0].mod, 0);
    vkDestroyShaderModule(dev, stages->shaders[1].mod, 0);
    free(stages);
}

input_assembler* create_input_assembler(VkPrimitiveTopology topology)
{
    input_assembler* assembler = calloc(sizeof(input_assembler), 1);
    assembler->sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    assembler->topology = topology;
    return assembler;
}

viewport_state* create_viewport_state()
{
    viewport_state* state       = calloc(sizeof(viewport_state), 1);
    VkViewport*     viewport    = calloc(sizeof(VkViewport), 1);
    VkRect2D*       scissor     = calloc(sizeof(VkRect2D), 1);

    int x, y;
    glfwGetWindowSize(get_window()->window, &x, &y);
    state->sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewport->width         = x; //get_window()->x;
    viewport->height        = y; //get_window()->y;
    viewport->maxDepth      = 1.f;

    scissor->extent.width   = x; //get_window()->x;
    scissor->extent.height  = y; //get_window()->y;

    state->viewportCount    = 1;
    state->scissorCount     = 1;
    state->pScissors        = scissor;
    state->pViewports       = viewport;

    return state;
}

void free_viewport_state(viewport_state* state)
{
    free((void*)state->pViewports);
    free((void*)state->pScissors);
    free(state);
}

rasterizer* create_rasterizer_state(VkPolygonMode mode, int cull)
{
    rasterizer* rast = calloc(sizeof(rasterizer), 11);
    rast->sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rast->polygonMode = mode;
    rast->lineWidth = 1.f;
    rast->frontFace = VK_FRONT_FACE_CLOCKWISE;
    rast->cullMode = cull ? VK_CULL_MODE_BACK_BIT : VK_CULL_MODE_NONE;

    return rast;
}

multisampler* create_multisampler_state()
{
    multisampler* ms = calloc(sizeof(multisampler), 1);
    ms->sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    ms->sampleShadingEnable = 1;
    ms->rasterizationSamples = get_swapchain()->msaa;
    return ms;
}

depth_stencil_state* create_depth_stencil_state(int enable)
{
    depth_stencil_state* state = calloc(sizeof(depth_stencil_state), 1);
    state->sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    state->depthTestEnable = enable;
    state->depthWriteEnable = 1;
    state->depthCompareOp = VK_COMPARE_OP_LESS;

    return state;
}

color_blend_state* create_blend_state(int enable)
{
    color_blend_state* state = calloc(sizeof(color_blend_state), 1);
    VkPipelineColorBlendAttachmentState* attachment = calloc(sizeof(VkPipelineColorBlendAttachmentState), 1);
    state->sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    state->attachmentCount = 1;
    state->pAttachments = attachment;
    attachment->colorWriteMask = VK_COLOR_COMPONENT_R_BIT | 
    VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

    if(!enable) return state;

    attachment->blendEnable = 1;
    attachment->srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
    attachment->dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    attachment->colorBlendOp = VK_BLEND_OP_ADD;
    attachment->srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
    attachment->dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    attachment->alphaBlendOp = VK_BLEND_OP_ADD;

    return state;
}

void free_blend_state(color_blend_state* state)
{
    free((void*)state->pAttachments);
    free(state);
}

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
    pipeline_meta* meta)
{
    meta->stype = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    meta->nstage = 2;

    meta->shader_info               = create_shader_stage_info(vs, ps);
    meta->vertex_info               = create_vertex_input_layout_desc(vlayout);
    meta->assembler_info            = create_input_assembler(topology);
    meta->viewport_info             = create_viewport_state();
    meta->rasterizer_info           = create_rasterizer_state(mode, cull);
    meta->multisampler_info         = create_multisampler_state();
    meta->depth_stencil_info        = create_depth_stencil_state(depth);
    meta->blend_info                = create_blend_state(blend);
    meta->pass                      = get_swapchain()->pass;

    VkPushConstantRange range = { stage_vertex | stage_frag, 0, 256 };
    VkPipelineLayoutCreateInfo info = { VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO };
    info.setLayoutCount         = nsets;
    info.pSetLayouts            = layouts;
    info.pushConstantRangeCount = 1;
    info.pPushConstantRanges    = &range;

    vkCreatePipelineLayout(get_device()->device, &info, 0, &meta->layout);
}

void destroy_pipeline_meta(pipeline_meta* meta)
{
    free_shader_stage_info(meta->shader_info);
    free_vertex_input_layout_desc(meta->vertex_info);
    free(meta->assembler_info);
    free_viewport_state(meta->viewport_info);
    free(meta->rasterizer_info);
    free(meta->multisampler_info);
    free(meta->depth_stencil_info);
    free_blend_state(meta->blend_info);
}