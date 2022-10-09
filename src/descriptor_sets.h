#pragma once

#include "defines.h"
#include "vulkan_functions.h"
#include "vulkan_types.h"
#include "buffer_types.h"
#include "textures.h"


VkDescriptorSetLayoutBinding layout_binding(
                                            VkDescriptorType type,
                                            VkShaderStageFlags shaderStages,
                                            uint32_t count,
                                            uint32_t bindingNumber)
{
    VkDescriptorSetLayoutBinding binding = {};
    binding.binding = bindingNumber;
    binding.descriptorCount = count;
    binding.descriptorType = type;
    binding.stageFlags = shaderStages;
    
    return binding;
}

static Descriptor *vk_create_descriptor( asset_type_id asset_id, descriptor_set_types * descriptor_context, logical_device_types * logical_context, sampler_types * sampler_context);

PINLINE VkDescriptorSetLayoutCreateInfo create_descriptor_layout(  u32 binding_count, const VkDescriptorSetLayoutBinding* layout_binding){
    
    VkDescriptorSetLayoutCreateInfo create_info= {}; 
    
    create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    //create_info.pNext = pnext;
    //create_info.flags = flags;
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
    
    VkDescriptorSetLayoutBinding layout_bindings[] =  {
        
        /*create_descriptor_layout_binding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                                         1, VK_SHADER_STAGE_VERTEX_BIT, NULL),
        create_descriptor_layout_binding(1, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
                                         1, VK_SHADER_STAGE_VERTEX_BIT, NULL),
        create_descriptor_layout_binding(2, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                                         1, VK_SHADER_STAGE_FRAGMENT_BIT, NULL),
        create_descriptor_layout_binding(3, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
                                         1, VK_SHADER_STAGE_FRAGMENT_BIT, NULL),
        
        */
        layout_binding(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT, 1, 0),
        layout_binding(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_VERTEX_BIT, 1, 1),
        layout_binding(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT, 1, 2),
        layout_binding(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_FRAGMENT_BIT, 1, 3)};
    
    
    
    VkDescriptorSetLayoutCreateInfo layout_info = create_descriptor_layout(  ArraySize(layout_bindings), layout_bindings);
    
    VK_CHECK(vkCreateDescriptorSetLayout(logical_context->logical_device, &layout_info, 0, &descriptor_context->set_layout));
    
    return VK_SUCCESS;
};

PINLINE VkResult create_descriptor_pool(logical_device_types * logical_context, descriptor_set_types * set_context){
    
    
    
    VkDescriptorPoolSize pool_sizes[] = {
        
        {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 10},
        {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 10},
        {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 10},
        //{VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1},
        
    };
    
    
    
    VkDescriptorPoolCreateInfo pool_info = {};
    pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    pool_info.maxSets = 100;
    pool_info.poolSizeCount = ArraySize(pool_sizes);
    pool_info.pPoolSizes = pool_sizes;
    VK_CHECK(vkCreateDescriptorPool(logical_context->logical_device, &pool_info, 0 , &set_context->descriptor_pool));
    
    
    return VK_SUCCESS;
};
/*
PINLINE VkResult create_descriptor_set(logical_device_types * logical_context, descriptor_set_types * set_context, sampler_types * sampler_context){
    
    VkDescriptorSetAllocateInfo allocate_info = {};
    allocate_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocate_info.pSetLayouts = &set_context->set_layout;
    allocate_info.descriptorSetCount = 1;
    allocate_info.descriptorPool = set_context->descriptor_pool;
    
    VK_CHECK(vkAllocateDescriptorSets(logical_context->logical_device, &allocate_info, &set_context->descriptor_set));
    Image * int_image = get_image(ASSET_SPRITE_WHITE);
    DescriptorInfo desc_infos[] = {
        
        DescriptorInfo(globalUBO.buffer ),
        DescriptorInfo(transform_storage_buffer.buffer),
        DescriptorInfo(sampler_context->sampler, int_image->view),
        
    };
    
    VkWriteDescriptorSet writes[] = {
        
        write_set(set_context->descriptor_set, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, &desc_infos[0], 0, 1),
        
        write_set(set_context->descriptor_set, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, &desc_infos[1], 1, 1),
        
        write_set(set_context->descriptor_set, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, &desc_infos[2], 2, 1),
        
        write_set(set_context->descriptor_set, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
                  &desc_infos[3], 3, 1)
    };
    
    vkUpdateDescriptorSets(logical_context->logical_device, ArraySize(writes), writes, 0, 0);
    
    
    return VK_SUCCESS;
    
};

*/

static Descriptor *vk_create_descriptor( asset_type_id asset_id, descriptor_set_types * descriptor_context, logical_device_types * logical_context, sampler_types * sampler_context)
{
    Descriptor *desc = 0;
    
    if (descriptor_count < MAX_DESCRIPTORS)
    {
        desc = &descriptors[descriptor_count];
        *desc = {};
        desc->asset_id = asset_id;
        
        // Allocation
        {
            VkDescriptorSetAllocateInfo allocInfo = {};
            allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
            allocInfo.pNext = NULL;
            allocInfo.pSetLayouts = &descriptor_context->set_layout;
            allocInfo.descriptorSetCount = 1;
            allocInfo.descriptorPool = descriptor_context->descriptor_pool;
            
            VK_CHECK(vkAllocateDescriptorSets(logical_context->logical_device, &allocInfo, &desc->set));
        }
        
        if (desc->set != VK_NULL_HANDLE)
        {
            // Update Descriptor Set
            {
                Image *image = get_image( ASSET_SPRITE_FONT_ATLAS);
                
                DescriptorInfo descInfos[] = {
                    DescriptorInfo(globalUBO.buffer),
                    DescriptorInfo(transform_storage_buffer.buffer),
                    DescriptorInfo(sampler_context->sampler, image->view),
                    DescriptorInfo(material_storage_buffer.buffer)};
                
                VkWriteDescriptorSet writes[] = {
                    write_set(desc->set, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                              &descInfos[0], 0, 1),
                    write_set(desc->set, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
                              &descInfos[1], 1, 1),
                    write_set(desc->set, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                              &descInfos[2], 2, 1),
                    write_set(desc->set, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
                              &descInfos[3], 3, 1),
                };
                
                vkUpdateDescriptorSets(logical_context->logical_device, ArraySize(writes), writes, 0, 0);
                
                // Actually add the descriptor;
                descriptor_count++;
            }
        }
        else
        {
            desc = 0;
            /*CAKEZ_ASSERT(0, "Failed to Allocate Descriptor Set for AssetTypeID: %d", assetTypeID);
            CAKEZ_ERROR("Failed to Allocate Descriptor Set for AssetTypeID: %d", assetTypeID);*/
        }
    }
    else
    {
        //CAKEZ_ASSERT(0, "Reached Maximum amount of Descriptors");
    };
    return desc;
}

PINLINE Descriptor *vk_get_descriptor( asset_type_id  assetTypeID)
{
    Descriptor *desc = 0;
    
    for (uint32_t i = 0; i < descriptor_count; i++)
    {
        Descriptor *d = &descriptors[i];
        
        if (d->asset_id == assetTypeID)
        {
            desc = d;
            break;
        }
    }
    
    if (!desc)
    {
        desc = vk_create_descriptor( assetTypeID, &descriptor_context, &logical_device_context, &sampler_context );
    }
    
    return desc;
}