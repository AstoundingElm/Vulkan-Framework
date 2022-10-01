#pragma once

#include "defines.h"
#include "vulkan_functions.h"
#include "vulkan_types.h"
#include "command_buffers.h"
#include "vulkan_images.h"

#include "assets.h"
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <cstring>



VkFenceCreateInfo fence_info(VkFenceCreateFlags flags = 0)
{
    VkFenceCreateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    info.flags = flags;
    return info;
}

#define INVALID_IDX UINT32_MAX

u32 findMemoryType(u32 typeFilter, VkMemoryPropertyFlags properties, physical_device_types * physical_context) {
    
    VkPhysicalDeviceMemoryProperties memProperties;
    
    vkGetPhysicalDeviceMemoryProperties(physical_context->physical_device_best_device, &memProperties);
    
    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
        if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
            return i;
        }
    }
    
    return false;
    //throw //std::runtime_error("failed to find suitable memory type!");
}

VkCommandBuffer beginSingleTimeCommands(logical_device_types * logical_context, command_pool_types * command_context) {
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = command_context->command_pool;
    allocInfo.commandBufferCount = 1;
    
    VkCommandBuffer commandBuffer;
    vkAllocateCommandBuffers(logical_context->logical_device, &allocInfo, &commandBuffer);
    
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    
    vkBeginCommandBuffer(commandBuffer, &beginInfo);
    
    return commandBuffer;
}

PINLINE VkResult load_texture(tex_file_data * load_data){
    
    load_data->pixels  = stbi_load("/home/petermiller/Desktop/4coder/hello/src/download", &load_texture_data.tex_width, &load_texture_data.tex_height, &load_texture_data.tex_channels, STBI_rgb_alpha);
    if(!load_data->pixels){
        return VK_ERROR_OUT_OF_HOST_MEMORY;
    }
    
    
    return  VK_SUCCESS;
    
    
};

void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory, logical_device_types * logical_context) {
    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = size;
    bufferInfo.usage = usage;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    
    if (vkCreateBuffer(logical_context->logical_device, &bufferInfo, nullptr, &buffer) != VK_SUCCESS) {
        //throw std::runtime_error("failed to create buffer!");
    }
    
    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(logical_context->logical_device, buffer, &memRequirements);
    
    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties, &physical_device_context);
    
    if (vkAllocateMemory(logical_context->logical_device, &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS) {
        //std::runtime_error("failed to allocate buffer memory!");
    }
    
    vkBindBufferMemory(logical_context->logical_device, buffer, bufferMemory, 0);
};

void createImage(u32 width, u32 height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory, logical_device_types * logical_context) {
    
    VkImageCreateInfo imageInfo{};
    imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.imageType = VK_IMAGE_TYPE_2D;
    imageInfo.extent.width = width;
    imageInfo.extent.height = height;
    imageInfo.extent.depth = 1;
    imageInfo.mipLevels = 1;
    imageInfo.arrayLayers = 1;
    imageInfo.format = format;
    imageInfo.tiling = tiling;
    imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageInfo.usage = usage;
    imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    
    if (vkCreateImage(logical_context->logical_device, &imageInfo, nullptr, &image) != VK_SUCCESS) {
        //throw std::runtime_error("failed to create image!");
    }
    
    VkMemoryRequirements memRequirements;
    vkGetImageMemoryRequirements(logical_context->logical_device, image, &memRequirements);
    
    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties, &physical_device_context);
    
    if (vkAllocateMemory(logical_context->logical_device, &allocInfo, nullptr, &imageMemory) != VK_SUCCESS) {
        //std::runtime_error("failed to allocate image memory!");
    }
    
    vkBindImageMemory(logical_context->logical_device, image, imageMemory, 0);
};


void endSingleTimeCommands(VkCommandBuffer commandBuffer, queue_types * queue_context, logical_device_types * logical_context,command_pool_types * command_context
                           ) {
    VK_CHECK(vkEndCommandBuffer(commandBuffer));
    
    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;
    
    VK_CHECK(vkQueueSubmit(queue_context->graphics_queue, 1, &submitInfo, VK_NULL_HANDLE));
    vkQueueWaitIdle(queue_context->graphics_queue);
    
    vkFreeCommandBuffers(logical_context->logical_device, command_context->command_pool, 1, &commandBuffer);
    
    
}
void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout) {
    VkCommandBuffer commandBuffer = beginSingleTimeCommands(&logical_device_context,&command_context);
    
    VkImageMemoryBarrier barrier{};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.oldLayout = oldLayout;
    barrier.newLayout = newLayout;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.image = image;
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = 1;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;
    
    VkPipelineStageFlags sourceStage = 0;
    VkPipelineStageFlags destinationStage = 0;
    
    if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        
        sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    } else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
        
        sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    } else {
        //std::invalid_argument("unsupported layout transition!");
    }
    
    vkCmdPipelineBarrier(
                         commandBuffer,
                         sourceStage, destinationStage,
                         0,
                         0, nullptr,
                         0, nullptr,
                         1, &barrier
                         );
    
    endSingleTimeCommands(commandBuffer, &q_context,&logical_device_context, &command_context );
}

void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height) {
    VkCommandBuffer commandBuffer = beginSingleTimeCommands(&logical_device_context, &command_context);
    
    VkBufferImageCopy region{};
    region.bufferOffset = 0;
    region.bufferRowLength = 0;
    region.bufferImageHeight = 0;
    region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    region.imageSubresource.mipLevel = 0;
    region.imageSubresource.baseArrayLayer = 0;
    region.imageSubresource.layerCount = 1;
    region.imageOffset = {0, 0, 0};
    region.imageExtent = {
        width,
        height,
        1
    };
    
    vkCmdCopyBufferToImage(commandBuffer, buffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);
    
    endSingleTimeCommands(commandBuffer, &q_context, &logical_device_context, &command_context);
}


PINLINE VkResult create_image_view(logical_device_types * logical_context, Image * image){
    
    VkImageViewCreateInfo view_info = create_image_view_info(NULL,0,  image->image, VK_IMAGE_VIEW_TYPE_2D, VK_FORMAT_R8G8B8A8_UNORM );
    VK_CHECK(vkCreateImageView(logical_context->logical_device, &view_info, 0, &image->view));
    return VK_SUCCESS;
};


PINLINE Image * material_image(logical_device_types * logical_context, physical_device_types * physical_context, command_pool_types * command_context, tex_file_data * load_data, queue_types * queue_context,  asset_type_id id){
    Image * image = 0;
    if(image_count < MAX_IMAGES){
        
        const char * actual_image_path = get_asset(id);
        load_data->tex_width = 1;
        load_data->tex_height = 1;
        VkDeviceSize image_size;
        if(id != ASSET_SPRITE_WHITE){
            
            load_data->pixels =stbi_load(actual_image_path, &load_data->tex_width,& load_data->tex_height, &load_data->tex_channels, STBI_rgb_alpha);
            
            
            
            if(!load_texture_data.pixels){
                printf("failed to load material iamge");
            }
            
            
            
            
        };
        
        
        image_size = load_data->tex_width * load_data->tex_height * 4;
        
        Buffer staging_buffer;
        
        createBuffer(image_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, staging_buffer.buffer, staging_buffer.memory, &logical_device_context);
        
        void* data;
        vkMapMemory(logical_context->logical_device, staging_buffer.memory, 0, image_size, 0, &data);
        memcpy(data, load_data->pixels, (size_t)image_size);
        vkUnmapMemory(logical_context->logical_device, staging_buffer.memory);
        
        stbi_image_free(load_data->pixels);
        image = &tex_images[image_count++];
        createImage(load_data->tex_width, load_data->tex_height, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,image->image, image->memory, &logical_device_context);
        
        if(image->image && image->memory){
            
            transitionImageLayout(image->image, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
            
            copyBufferToImage(staging_buffer.buffer, image->image, (u32)load_data->tex_width, (u32)load_data->tex_height);
            
            transitionImageLayout(image->image, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
            
            vkDestroyBuffer(logical_context->logical_device, staging_buffer.buffer, nullptr);
            vkFreeMemory(logical_context->logical_device, staging_buffer.memory, nullptr);
        }else{
            
            puts("failed too allocate iage ptr");
            image_count--;
            image_count = 0;
        };
    }else{
        printf("reached max amount of images");
        
        
    };
    return image;
    
    
};

PINLINE VkResult create_texture(logical_device_types * logical_context, physical_device_types * physical_context,
                                command_pool_types * command_context,
                                tex_file_data * tex_data, queue_types * queue_context){
    tex_data->pixels  = stbi_load("/home/petermiller/Desktop/4coder/hello/src/download.jpeg", &tex_data->tex_width,& tex_data->tex_height, &tex_data->tex_channels, STBI_rgb_alpha);
    
    
    VkDeviceSize image_size;
    image_size = load_texture_data.tex_width * load_texture_data.tex_height * 4;
    
    
    if(!load_texture_data.pixels){
        return VK_ERROR_OUT_OF_HOST_MEMORY;
    }
    
    Buffer staging_buffer;
    
    createBuffer(image_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, staging_buffer.buffer, staging_buffer.memory, &logical_device_context);
    
    void* data;
    vkMapMemory(logical_context->logical_device, staging_buffer.memory, 0, image_size, 0, &data);
    memcpy(data, load_texture_data.pixels, (size_t)image_size);
    vkUnmapMemory(logical_context->logical_device, staging_buffer.memory);
    
    stbi_image_free(load_texture_data.pixels);
    
    createImage(load_texture_data.tex_width, load_texture_data.tex_height, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,tex_image.image, tex_image.memory, &logical_device_context);
    
    
    transitionImageLayout(tex_image.image, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
    
    copyBufferToImage(staging_buffer.buffer, tex_image.image, (u32)load_texture_data.tex_width, (u32)load_texture_data.tex_height);
    transitionImageLayout(tex_image.image, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    
    vkDestroyBuffer(logical_context->logical_device, staging_buffer.buffer, nullptr);
    vkFreeMemory(logical_context->logical_device, staging_buffer.memory, nullptr);
    return VK_SUCCESS;
    
    
    
};

uint32_t vk_get_memory_type_index(
                                  VkPhysicalDevice gpu,
                                  VkMemoryRequirements memRequirements,
                                  VkMemoryPropertyFlags memProps)
{
    uint32_t typeIdx = INVALID_IDX;
    
    VkPhysicalDeviceMemoryProperties gpuMemProps;
    vkGetPhysicalDeviceMemoryProperties(gpu, &gpuMemProps);
    
    VkMemoryAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    for (uint32_t i = 0; i < gpuMemProps.memoryTypeCount; i++)
    {
        if (memRequirements.memoryTypeBits & (1 << i) &&
            (gpuMemProps.memoryTypes[i].propertyFlags & memProps) == memProps)
        {
            typeIdx = i;
            break;
        }
    }
    
    //CAKEZ_ASSERT(typeIdx != INVALID_IDX, "Failed to fine proper type Index for Memory Properties: %d", memProps);
    
    return typeIdx;
}

Image vk_allocate_image(
                        VkDevice device,
                        VkPhysicalDevice gpu,
                        uint32_t width,
                        uint32_t height,
                        VkFormat format)
{
    Image image = {};
    
    VkImageCreateInfo imgInfo = {};
    imgInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imgInfo.mipLevels = 1;
    imgInfo.arrayLayers = 1;
    imgInfo.imageType = VK_IMAGE_TYPE_2D;
    imgInfo.format = format;
    imgInfo.extent = {width, height, 1};
    imgInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    imgInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
    // imgInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    
    VK_CHECK(vkCreateImage(device, &imgInfo, 0, &image.image));
    
    VkMemoryRequirements memRequirements;
    vkGetImageMemoryRequirements(device, image.image, &memRequirements);
    
    VkMemoryAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = vk_get_memory_type_index(gpu, memRequirements, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    
    VK_CHECK(vkAllocateMemory(device, &allocInfo, 0, &image.memory));
    VK_CHECK(vkBindImageMemory(device, image.image, image.memory, 0));
    
    return image;
}

Buffer vk_allocate_buffer(
                          VkDevice device,
                          VkPhysicalDevice gpu,
                          uint32_t size,
                          VkBufferUsageFlags bufferUsage,
                          VkMemoryPropertyFlags memProps)
{
    Buffer buffer = {};
    buffer.size = size;
    
    VkBufferCreateInfo bufferInfo = {};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.usage = bufferUsage;
    bufferInfo.size = size;
    VK_CHECK(vkCreateBuffer(device, &bufferInfo, 0, &buffer.buffer));
    
    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(device, buffer.buffer, &memRequirements);
    
    VkMemoryAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = vk_get_memory_type_index(gpu, memRequirements, memProps);
    
    VK_CHECK(vkAllocateMemory(device, &allocInfo, 0, &buffer.memory));
    
    // Only map memory we can actually write to from the CPU
    if (memProps & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT)
    {
        VK_CHECK(vkMapMemory(device, buffer.memory, 0, memRequirements.size, 0, &buffer.data));
    }
    
    VK_CHECK(vkBindBufferMemory(device, buffer.buffer, buffer.memory, 0));
    
    return buffer;
}


VkSamplerCreateInfo create_sampler_info(const void * pnext, VkSamplerCreateFlags flags, VkFilter mag_filter, VkFilter min_filter
                                        /*, VkSamplerMipmapMode mip_map_mode, VkSamplerAddressMode address_mode_u,
                                        VkSamplerAddressMode address_mode_v,
                                        VkSamplerAddressMode address_mode_w, 
                                        float mip_lod_bias,VkBool32 anisotropy_enable, VkCompareOp compare_op, float min_lod, float max_lod, VkBorderColor border_colour, VkBool32 unnormalized_coordinates*/){
    
    VkSamplerCreateInfo create_info = {};
    create_info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    create_info.pNext = pnext;
    create_info.flags = flags;
    create_info.magFilter = mag_filter;
    create_info.minFilter = min_filter;
    /*create_info.mipmapMode = mip_map_mode;
    create_info.addressModeU = address_mode_u;
    create_info.addressModeV = address_mode_v;
    create_info.addressModeW = address_mode_w;
    create_info.mipLodBias = mip_lod_bias;
    create_info.anisotropyEnable = anisotropy_enable;
    create_info.maxAnisotropy = max_anisotropy;
    create_info.compareEnable = compare_enable;
    create_info.compareOp = compare_op;
    create_info.minLod = min_lod;
    create_info.maxLod = max_lod;
    create_info.borderColor = border_colour;
    create_info.unnormalizedCoordinate = unnormalized_coordinates;
    */
    return create_info;
    
};


PINLINE VkResult create_sampler(logical_device_types * logical_context, sampler_types * sampler_context){
    
    VkSamplerCreateInfo sampler_info  = create_sampler_info(NULL, 0, VK_FILTER_NEAREST, VK_FILTER_NEAREST  );
    VK_CHECK(vkCreateSampler(logical_context->logical_device, &sampler_info, 0 ,&sampler_context->sampler ));
    
    return VK_SUCCESS;
};




