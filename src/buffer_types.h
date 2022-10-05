#pragma once

#include "defines.h"
#include "vulkan_functions.h"
#include "vulkan_types.h"
#include "assets.h"



#define MAX_IMAGES 100
struct Image
{
    VkImage image;
    VkImageView view;
    VkDeviceMemory memory;
    asset_type_id asset_id;
};

struct Buffer{
    
    VkBuffer buffer;
    VkDeviceMemory memory;
    u32 size;
    void * data;
};

PINLINE Image * material_image(logical_device_types * logical_context,  tex_file_data * load_data,  asset_type_id id);


static u32 image_count;
global_variable Buffer transform_storage_buffer;
global_variable Buffer globalUBO;
global_variable Buffer indexBuffer;
global_variable Image tex_image;
global_variable Image tex_images[MAX_IMAGES];
global_variable Buffer material_storage_buffer;
//global_variable Buffer staging_buffer = {};
//global_variable Buffer internal_alloc_buffer;
//global_variable Buffer actual_buffer;
static Image * global_image = {};

Image * get_image(asset_type_id id){
    Image * image = 0;
    for(u32 i = 0; i < image_count; i++){
        
        Image * img = &tex_images[i];
        if(img->asset_id == id){
            
            image = img;
            break;
        }
    }
    if (!image)
    {
        image = material_image(&logical_device_context, &load_texture_data, ASSET_SPRITE_DEFAULT);
    }
    
    
    return image;
};
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
