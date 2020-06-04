#pragma once
#include <vulkan/vulkan.h>
#include "GLFW/glfw3.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "vec_types.h"

typedef unsigned uint;
typedef unsigned long long uint64;

#define LOCAL_MEMORY 7
#define SHARED_MEMORY 9
#define SHARED_MEMORY_CACHED 10

typedef struct device {
    VkDevice            device;
    VkInstance          instance;
    VkPhysicalDevice    p_device;
    VkQueue             queue[3];
    VkDebugUtilsMessengerEXT messenger;
} device;

void destroy_device();
void create_device();
device* get_device();

VkMemoryRequirements buffer_memory_req(VkBuffer buffer);
VkMemoryRequirements image_memory_req(VkImage buffer);

enum queueID
{
	RenderQueue = 0,
	ComputeQueue = 1,
	CopyQueue = 2
};

enum buffer_usage
{
    usage_src			= VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
    usage_dst			= VK_BUFFER_USAGE_TRANSFER_DST_BIT,
    usage_uniform_texel = VK_BUFFER_USAGE_UNIFORM_TEXEL_BUFFER_BIT,
    usage_storage_texel = VK_BUFFER_USAGE_STORAGE_TEXEL_BUFFER_BIT,
    usage_uniform		= VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
    usage_storage		= VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
    usage_index			= VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
    usage_vertex		= VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
    usage_indirect		= VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT
};

enum shader_stage
{
	stage_vertex				= VK_SHADER_STAGE_VERTEX_BIT,
	stage_tessellation_control	= VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT,
	stage_tessellation_eval		= VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT,
	stage_geometry				= VK_SHADER_STAGE_GEOMETRY_BIT,
	stage_frag					= VK_SHADER_STAGE_FRAGMENT_BIT,
	stage_compute				= VK_SHADER_STAGE_COMPUTE_BIT,
	stage_all_graphics			= VK_SHADER_STAGE_ALL_GRAPHICS,
	stage_all					= VK_SHADER_STAGE_ALL,
};

enum desc_type
{
	desc_sampler				= VK_DESCRIPTOR_TYPE_SAMPLER,
	desc_combined_image_sampler = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
	desc_sampled_image			= VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,
	desc_storage_image			= VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,
	desc_texture				= VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,
	desc_uniform				= VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
	desc_uniform_dynamic		= VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC
};


enum vertex_input_type_format
{
	format_uint		= VK_FORMAT_R32_UINT,
	format_int		= VK_FORMAT_R32_SINT,
	format_float	= VK_FORMAT_R32_SFLOAT,

	format_float4 	= VK_FORMAT_R32G32B32A32_SFLOAT,
	format_float3 	= VK_FORMAT_R32G32B32_SFLOAT,
	format_float2 	= VK_FORMAT_R32G32_SFLOAT,
	format_int4 	= VK_FORMAT_R32G32B32A32_SINT,
	format_int3 	= VK_FORMAT_R32G32B32_SINT,
	format_int2 	= VK_FORMAT_R32G32_SINT,
	format_uint4 	= VK_FORMAT_R32G32B32A32_UINT,
	format_uint3 	= VK_FORMAT_R32G32B32_UINT,
	format_uint2 	= VK_FORMAT_R32G32_UINT,
};

enum primitive_topology
{
	point_list  		= VK_PRIMITIVE_TOPOLOGY_POINT_LIST,
    line_list  			= VK_PRIMITIVE_TOPOLOGY_LINE_LIST,
    line_strip  		= VK_PRIMITIVE_TOPOLOGY_LINE_STRIP,
    triangle_list  		= VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
    triangle_strip  	= VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP,
    triangle_fan  		= VK_PRIMITIVE_TOPOLOGY_TRIANGLE_FAN,
};

enum polygon_mode
{
	polygon_fill = 0,
	polygon_wireframe = 1
};

typedef enum queueID queueID;
typedef enum buffer_usage buffer_usage;
typedef enum shader_stage shader_stage;
typedef enum desc_type desc_type;