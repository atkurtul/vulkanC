#include "descriptor.h"

VkDescriptorPool create_descriptor_pool(uint npool, desc_pool_size* poolsizes)
{
    VkDescriptorPoolCreateInfo info = { VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO };
    info.maxSets = 255;
    info.poolSizeCount = npool;
    info.pPoolSizes = poolsizes;
    VkDescriptorPool pool;
    vkCreateDescriptorPool(get_device()->device, &info, 0, &pool);
    return pool;
}

VkDescriptorSet create_desc_set(VkDescriptorPool pool, VkDescriptorSetLayout layout)
{
    VkDescriptorSet set;
    VkDescriptorSetAllocateInfo info = { VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO };
    info.descriptorPool = pool;
    info.pSetLayouts = &layout;
    info.descriptorSetCount = 1;
    vkAllocateDescriptorSets(get_device()->device, &info, &set);
    return set;
}


VkDescriptorSetLayoutBinding init_binding(
    desc_type type,
	shader_stage stage,
	uint bind,
	uint descriptorCount)
{
    VkDescriptorSetLayoutBinding binding = {};
	binding.descriptorCount = descriptorCount;
	binding.binding = bind;
	binding.stageFlags = stage;
	binding.descriptorType = type;
    return binding;
}   

desc_set_layout create_desc_set_layout(desc_type type, shader_stage stage)
{
    desc_set_layout layout;
	VkDescriptorSetLayoutBinding bind = init_binding(type, stage, 0, 1);
	VkDescriptorSetLayoutCreateInfo info = { VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO };
    info.bindingCount = 1;
    info.pBindings = &bind;
	vkCreateDescriptorSetLayout(get_device()->device, &info, 0, &layout);
    return layout;
}

desc_set_layout create_desc_set_layout_table(uint nbindings, desc_type* types, shader_stage* stages)
{
    desc_set_layout layout;
    VkDescriptorSetLayoutBinding* bindings = calloc(sizeof(VkDescriptorSetLayoutBinding), nbindings);
    for (int i = 0; i < nbindings; ++i)
        bindings[i] = init_binding(types[i], stages[i], i, 1);
    VkDescriptorSetLayoutCreateInfo info = { VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO };
    info.bindingCount = 1;
    info.pBindings = bindings;
	vkCreateDescriptorSetLayout(get_device()->device, &info, 0, &layout);
    free(bindings);
    return layout;
}

void set_bind_buffer(VkDescriptorSet set, VkBuffer buffer, desc_type type, uint binding)
{
    VkWriteDescriptorSet write = { VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET };
    write.descriptorCount = 1;
    write.descriptorType = type;
    write.dstBinding = binding;
    write.dstSet = set;
    VkDescriptorBufferInfo info = { buffer, 0, -1 };
	write.pBufferInfo = &info;
	vkUpdateDescriptorSets(get_device()->device, 1, &write, 0, 0);
}

void set_bind_texture(VkDescriptorSet set, texture* tex, uint binding)
{
    VkWriteDescriptorSet write = { VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET };
    write.descriptorCount = 1;
    write.descriptorType = desc_combined_image_sampler;
    write.dstBinding = binding;
    write.dstSet = set;

    VkDescriptorImageInfo info = { tex->sampler, tex->buffer.view, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL };
    write.pImageInfo = &info;
	vkUpdateDescriptorSets(get_device()->device, 1, &write, 0, 0);
}