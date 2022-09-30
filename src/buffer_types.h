#pragma once

#include "defines.h"
#include "vulkan_functions.h"
#include "vulkan_types.h"


struct Image
{
    VkImage image;
    VkImageView view;
    VkDeviceMemory memory;
};

struct Buffer{
    
    VkBuffer buffer;
    VkDeviceMemory memory;
    u32 size;
    void * data;
};



global_variable Buffer transform_storage_buffer;
global_variable Buffer globalUBO;
global_variable Buffer indexBuffer;
global_variable Image tex_image;
//global_variable Buffer staging_buffer = {};
//global_variable Buffer internal_alloc_buffer;
//global_variable Buffer actual_buffer;


void vk_copy_to_buffer(Buffer *buffer, void *data, uint32_t size)
{
    /*CAKEZ_ASSERT(buffer->size >= size, "Buffer too small: %d for data: %d", buffer->size, size);
    */
    // If we have mapped data
    if (buffer->data)
    {
        memcpy(buffer->data, data, size);
    }
    else
    {
        
        // TODO: Implement, copy data using command buffer
    }
}
