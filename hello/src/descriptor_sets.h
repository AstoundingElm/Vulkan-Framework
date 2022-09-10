#pragma once

#include "defines.h"
#include "vulkan_functions.h"
#include "vulkan_types.h"
#include "buffer_types.h"

PINLINE VkDescriptorSetLayoutCreateInfo create_descriptor_layout(const void * pnext, VkDescriptorSetLayoutCreateFlags flags, u32 binding_count, const VkDescriptorSetLayoutBinding* layout_binding){
    
    VkDescriptorSetLayoutCreateInfo create_info = {VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO};
    create_info.pNext = pnext;
    create_info.flags = flags;
    create_info.bindingCount = binding_count;
    create_info.pBindings = layout_binding;
    
    return create_info;
    
};

PINLINE VkDescriptorSetLayoutBinding create_descriptor_layout_binding(u32 binding, VkDescriptorType descriptor_type,
                                                                      u32 descriptor_count, VkShaderStageFlags stage_flags, const VkSampler * immutable_samplers){
    
    VkDescriptorSetLayoutBinding layout_binding;
    layout_binding.binding = binding;
    layout_binding.descriptorType = descriptor_type;
    layout_binding.descriptorCount = descriptor_count;
    layout_binding.stageFlags = stage_flags;
    layout_binding.pImmutableSamplers = immutable_samplers;
    return layout_binding;
    
};

PINLINE VkResult create_descriptor_sets(logical_device_types * logical_context, descriptor_set_types * descriptor_context){
    
    VkDescriptorSetLayoutBinding binding = create_descriptor_layout_binding(0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                                                                            1, VK_SHADER_STAGE_FRAGMENT_BIT, NULL);
    
    VkDescriptorSetLayoutCreateInfo layout_info = create_descriptor_layout(NULL, 0, 1, &binding);
    
    VK_CHECK(vkCreateDescriptorSetLayout(logical_context->logical_device, &layout_info, 0, &descriptor_context->set_layout));
    
    return VK_SUCCESS;
};

PINLINE VkResult create_descriptor_pool(logical_device_types * logical_context, descriptor_set_types * set_context){
    
    VkDescriptorPoolSize pool_size = {};
    pool_size.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    pool_size.descriptorCount = 1;
    
    VkDescriptorPoolCreateInfo pool_info = {};
    pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    pool_info.maxSets = 1;
    pool_info.poolSizeCount = 1;
    pool_info.pPoolSizes = &pool_size;
    VK_CHECK(vkCreateDescriptorPool(logical_context->logical_device, &pool_info, 0 , &set_context->descriptor_pool));
    
    return VK_SUCCESS;
};

PINLINE VkResult create_descriptor_set(logical_device_types * logical_context, descriptor_set_types * set_context, sampler_types * sampler_context){
    
    VkDescriptorSetAllocateInfo allocate_info = {};
    allocate_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocate_info.pSetLayouts = &set_context->set_layout;
    allocate_info.descriptorSetCount = 1;
    allocate_info.descriptorPool = set_context->descriptor_pool;
    
    VK_CHECK(vkAllocateDescriptorSets(logical_context->logical_device, &allocate_info, &set_context->descriptor_set));
    VkDescriptorImageInfo image_info = {};
    image_info.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    image_info.imageView = tex_image.view;
    image_info.sampler = sampler_context->sampler;
    
    VkWriteDescriptorSet write = {};
    write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    write.dstSet = set_context->descriptor_set;
    write.pImageInfo = &image_info;
    write.dstBinding = 0;
    write.descriptorCount = 1;
    write.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    vkUpdateDescriptorSets(logical_context->logical_device, 1, &write, 0, 0);
    
    return VK_SUCCESS;
    
};