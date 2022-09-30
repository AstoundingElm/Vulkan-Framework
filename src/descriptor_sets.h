#pragma once

#include "defines.h"
#include "vulkan_functions.h"
#include "vulkan_types.h"
#include "buffer_types.h"
#include "textures.h"

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
    
    /*VkDescriptorSetLayoutBinding binding = create_descriptor_layout_binding(0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                                                                            1, VK_SHADER_STAGE_FRAGMENT_BIT, NULL);
    */
    VkDescriptorSetLayoutBinding layout_bindings[] =  {
        
        create_descriptor_layout_binding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                                         1, VK_SHADER_STAGE_VERTEX_BIT, NULL),
        create_descriptor_layout_binding(1, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
                                         1, VK_SHADER_STAGE_VERTEX_BIT, NULL),
        create_descriptor_layout_binding(2, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                                         1, VK_SHADER_STAGE_FRAGMENT_BIT, NULL),
        
        
    };
    
    VkDescriptorSetLayoutCreateInfo layout_info = create_descriptor_layout(NULL, 0, ArraySize(layout_bindings), layout_bindings);
    
    VK_CHECK(vkCreateDescriptorSetLayout(logical_context->logical_device, &layout_info, 0, &descriptor_context->set_layout));
    
    return VK_SUCCESS;
};

PINLINE VkResult create_descriptor_pool(logical_device_types * logical_context, descriptor_set_types * set_context){
    
    VkDescriptorPoolSize pool_size = {};
    pool_size.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    pool_size.descriptorCount = 1;
    indexBuffer = vk_allocate_buffer(
                                     logical_device_context.logical_device,
                                     physical_device_context.physical_device_best_device,
                                     sizeof(uint32_t) * 6,
                                     VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
                                     VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    
    /*createBuffer(sizeof(u32)* 6, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT| VK_BUFFER_USAGE_INDEX_BUFFER_BIT,  VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                 IndexBuffer.buffer, IndexBuffer.memory, &logical_device_context);
    */
    
    
    u32 indices[] = {
        0, 1, 2, 2, 3, 0,
    };
    
    vk_copy_to_buffer(&indexBuffer, &indices, sizeof(u32 ) * 6);
    
    VkDescriptorPoolSize pool_sizes[] = {
        
        {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1},
        {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1},
        {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1},
    };
    
    
    
    VkDescriptorPoolCreateInfo pool_info = {};
    pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    pool_info.maxSets = 1;
    pool_info.poolSizeCount = ArraySize(pool_sizes);
    pool_info.pPoolSizes = pool_sizes;
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
    
    DescriptorInfo desc_infos[] = {
        
        DescriptorInfo(globalUBO.buffer ),
        DescriptorInfo(transform_storage_buffer.buffer),
        DescriptorInfo(sampler_context->sampler, tex_image.view),
        
    };
    
    VkWriteDescriptorSet writes[] = {
        
        write_set(set_context->descriptor_set, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, &desc_infos[0], 0, 1),
        
        write_set(set_context->descriptor_set, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, &desc_infos[1], 1, 1),
        
        write_set(set_context->descriptor_set, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, &desc_infos[2], 2, 1),
    };
    
    /*VkDescriptorImageInfo image_info = {};
    image_info.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    image_info.imageView = tex_image.view;
    image_info.sampler = sampler_context->sampler;
    
    VkWriteDescriptorSet write = {};
    write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    write.dstSet = set_context->descriptor_set;
    write.pImageInfo = &desc_infos.imageInfo;
    //write.dstBinding = &desc_infos.bufferInfo;
    write.dstBinding = 0;
    write.descriptorCount = 1;
    write.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    */
    vkUpdateDescriptorSets(logical_context->logical_device, ArraySize(writes), writes, 0, 0);
    
    return VK_SUCCESS;
    
};