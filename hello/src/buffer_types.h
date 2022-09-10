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


global_variable Image tex_image;
//global_variable Buffer staging_buffer = {};
//global_variable Buffer internal_alloc_buffer;
//global_variable Buffer actual_buffer;
