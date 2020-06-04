#pragma once
#include "device.h"
#include "texture.h"

typedef struct desc_pool_size
{
	desc_type type;
	uint size;
} desc_pool_size;

typedef VkDescriptorPool desc_pool;
typedef VkDescriptorSet desc_set;
typedef VkDescriptorSetLayout desc_set_layout;

desc_pool create_descriptor_pool(uint npool, desc_pool_size* poolsizes);

desc_set create_desc_set(desc_pool pool, desc_set_layout layout);

desc_set_layout create_desc_set_layout(desc_type type, shader_stage stage);

desc_set_layout create_desc_set_layout_table(uint nbindings, desc_type* types, shader_stage* stages);

void set_bind_buffer(VkDescriptorSet set, VkBuffer buffer, desc_type type, uint binding);

void set_bind_texture(VkDescriptorSet set, texture* tex, uint binding);

