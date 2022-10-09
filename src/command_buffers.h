#pragma once

#include "defines.h"
#include "vulkan_functions.h"
#include "vulkan_types.h"
#include "buffer_types.h"
#include <string.h>

PINLINE VkCommandBufferAllocateInfo cmd_alloc_info(const void * pnext,VkCommandPool pool, VkCommandBufferLevel command_buffer_level, u32 command_buffer_count) {
    VkCommandBufferAllocateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    info.pNext = pnext;
    info.commandPool = pool;
    info.level = command_buffer_level;
    info.commandBufferCount = command_buffer_count;
    
    return info;
};

PINLINE VkCommandPoolCreateInfo create_command_pool_info(const void * pnext, VkCommandPoolCreateFlags command_flags, u32 queue_family_index) {
    VkCommandPoolCreateInfo command_pool_info = {};
    command_pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    command_pool_info.pNext = pnext;
    command_pool_info.flags = command_flags;
    command_pool_info.queueFamilyIndex = queue_family_index;
    
    return command_pool_info;
};

PINLINE VkResult create_command_pool(physical_device_types * physical_context, logical_device_types * logical_context, command_pool_types * command_context){
    
    
    VkCommandPoolCreateInfo pool_info = create_command_pool_info(NULL, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT, physical_context->graphics_index);
    
    VK_CHECK(vkCreateCommandPool(logical_context->logical_device, &pool_info, 0, &command_context->command_pool));
    VkCommandBufferAllocateInfo alloc_info = cmd_alloc_info(NULL, command_context->command_pool, VK_COMMAND_BUFFER_LEVEL_PRIMARY, 1 );
    VK_CHECK(vkAllocateCommandBuffers(logical_context->logical_device, &alloc_info, &command_context->command_buffer));
    return VK_SUCCESS;
};


PINLINE VkBufferCreateInfo create_buffer_info(const void * pnext, VkBufferCreateFlags buffer_flags, VkDeviceSize buffer_size, VkBufferUsageFlags buffer_usage, VkSharingMode buffer_sharing_mode, u32 buffer_fam_index_count, const u32 * buffer_indices){
    
    
    VkBufferCreateInfo create_info = {};
    create_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    create_info.pNext = pnext;
    create_info.flags = buffer_flags;
    create_info.size = buffer_size;
    create_info.usage = buffer_usage;
    create_info.sharingMode = buffer_sharing_mode;
    create_info.queueFamilyIndexCount = buffer_fam_index_count;
    create_info.pQueueFamilyIndices = buffer_indices;
    return create_info;
};

PINLINE VkMemoryAllocateInfo mem_alloc_info(const void * pnext, VkDeviceSize allocation_size, u32 memory_type_index){
    
    VkMemoryAllocateInfo mem_info = {};
    mem_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    mem_info.pNext = pnext;
    mem_info.allocationSize = allocation_size;
    mem_info.memoryTypeIndex = memory_type_index;
    
    return mem_info;
    
}

PINLINE u32 get_memory_type_index(VkPhysicalDevice gpu, VkMemoryRequirements memory_requirements, VkMemoryPropertyFlags mem_props){
    u32 type_index = UINT32_MAX;
    VkPhysicalDeviceMemoryProperties gpu_memory_properties;
    vkGetPhysicalDeviceMemoryProperties(gpu, &gpu_memory_properties);
    
    VkMemoryAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memory_requirements.size;
    for(u32 i = 0;i < gpu_memory_properties .memoryTypeCount; i++){
        
        if(memory_requirements.memoryTypeBits & (1 << i) && (gpu_memory_properties.memoryTypes[i].propertyFlags & mem_props) == mem_props){
            type_index = i;
            break;
        }
    }
    if(type_index == UINT32_MAX){
        u32 fake_index = 0;
        printf("failure");
        return fake_index;
    };
    
    return type_index;
    
};
