#pragma once

#include "defines.h"
#include "vulkan_functions.h"
#include "vulkan_types.h"
#include "buffer_types.h"
#include <string.h>

/*PINLINE VkCommandBufferBeginInfo cmd_begin_info() {
    VkCommandBufferBeginInfo info = {VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO};
    info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    
    return info;
};*/

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
    VkCommandPoolCreateInfo command_pool_info = {VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO};
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
    
    
    VkBufferCreateInfo create_info = {VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO};
    create_info.pNext = pnext;
    create_info.flags = buffer_flags;
    create_info.size = buffer_size;
    create_info.usage = buffer_usage;
    create_info.sharingMode = buffer_sharing_mode;
    create_info.queueFamilyIndexCount = buffer_fam_index_count;
    create_info.pQueueFamilyIndices = buffer_indices;
    return create_info;
};

VkMemoryAllocateInfo mem_alloc_info(const void * pnext, VkDeviceSize allocation_size, u32 memory_type_index){
    
    VkMemoryAllocateInfo mem_info = {VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO};
    mem_info.pNext = pnext;
    mem_info.allocationSize = allocation_size;
    mem_info.memoryTypeIndex = memory_type_index;
    
    return mem_info;
    
}

u32 get_memory_type_index(VkPhysicalDevice gpu, VkMemoryRequirements memory_requirements, VkMemoryPropertyFlags mem_props){
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

/*PINLINE Buffer allocate_buffer(VkDevice device, VkPhysicalDevice gpu, u32 size, VkBufferUsageFlags buffer_usage, VkMemoryPropertyFlags memory_properties){
    
    Buffer internal_alloc_buffer{};
    internal_alloc_buffer.size = size;
    VkBufferCreateInfo buffer_info = create_buffer_info(NULL, 0, size, buffer_usage,VK_SHARING_MODE_EXCLUSIVE, 0, NULL );
    
    
    
    VK_CHECK(vkCreateBuffer(device, &buffer_info, 0, &internal_alloc_buffer.buffer));

    
    VkBufferCreateInfo bufferInfo = {};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.usage = buffer_usage;
    bufferInfo.size = size;
    VK_CHECK(vkCreateBuffer(device, &bufferInfo, 0, &internal_alloc_buffer.buffer));
    VkMemoryRequirements memory_requirements;
    vkGetBufferMemoryRequirements(device,internal_alloc_buffer.buffer, &memory_requirements);
    
    VkMemoryAllocateInfo allocInfo;
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = internal_alloc_buffer.size;
    allocInfo.memoryTypeIndex = get_memory_type_index(gpu, memory_requirements, memory_properties);
    //mem_alloc_info(NULL, internal_alloc_buffer.size,get_memory_type_index(gpu, memory_requirements, memory_properties)); 
    VK_CHECK(vkAllocateMemory(device, &allocInfo, 0, &internal_alloc_buffer.memory));
    if(memory_properties & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT){
        
        VK_CHECK(vkMapMemory(device, internal_alloc_buffer.memory, 0, MB(1), 0, &internal_alloc_buffer.data));
        
    }
    
    VK_CHECK(vkBindBufferMemory(device, internal_alloc_buffer.buffer, internal_alloc_buffer.memory, 0));
    return internal_alloc_buffer;
    
};

PINLINE VkResult create_staging_buffer( logical_device_types * logical_context, physical_device_types * physical_context){
    
    staging_buffer = allocate_buffer(logical_context->logical_device, physical_context->physical_device_best_device, MB(1), VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
    
    
    return VK_SUCCESS;
    
};
*/