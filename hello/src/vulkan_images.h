#pragma once

#include "defines.h"
#include "vulkan_functions.h"
#include "vulkan_types.h"

PINLINE VkImageMemoryBarrier create_image_memory_barrier(const void * pnext, VkAccessFlags src_access_mask, VkAccessFlags dst_access_mask,VkImageLayout old_layout, VkImageLayout new_layout, u32 src_queue_family_index, u32 dst_queue_family_index, VkImage image, VkImageSubresourceRange image_subresource_range ){
    
    VkImageMemoryBarrier image_memory_barrier = {VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER};
    
    image_memory_barrier.pNext = pnext;
    image_memory_barrier.srcAccessMask = src_access_mask;
    image_memory_barrier.dstAccessMask = dst_access_mask;
    image_memory_barrier.oldLayout = old_layout;
    image_memory_barrier.newLayout = new_layout;
    image_memory_barrier.srcQueueFamilyIndex = src_queue_family_index;
    image_memory_barrier.dstQueueFamilyIndex = dst_queue_family_index;
    image_memory_barrier.image = image;
    image_memory_barrier.subresourceRange  = image_subresource_range;
    return image_memory_barrier;
}

PINLINE VkImageViewCreateInfo create_image_view_info(const void * pnext, VkImageViewCreateFlags flags, 
                                                     VkImage image, VkImageViewType viewtype, VkFormat format) {
    VkImageViewCreateInfo image_view_create_info = {};
    
    image_view_create_info.sType = {VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO};
    image_view_create_info.pNext = pnext;
    image_view_create_info.flags = flags;
    image_view_create_info.image = image;
    image_view_create_info.viewType = viewtype;
    image_view_create_info.format = format;
    //image_view_create_info.components  = components;
    image_view_create_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    image_view_create_info.subresourceRange.levelCount = 1;
    image_view_create_info.subresourceRange.layerCount = 1;
    return image_view_create_info;
    
};

PINLINE VkImageSubresourceRange create_image_subresource_range(VkImageAspectFlags aspect_mask, u32 base_mip_level, u32 level_count, 
                                                               u32 base_array_layer, u32 layer_count){
    VkImageSubresourceRange image_subresource_range;
    image_subresource_range.aspectMask = aspect_mask;
    image_subresource_range.baseMipLevel = base_mip_level;
    image_subresource_range.levelCount = level_count;
    image_subresource_range.baseArrayLayer = base_array_layer;
    image_subresource_range.layerCount = layer_count;
    
    return image_subresource_range;
    
};

PINLINE Image  allocate_image(VkDevice device, VkPhysicalDevice gpu, u32 width, u32 height, VkFormat format){
    
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
    allocInfo.memoryTypeIndex = get_memory_type_index(gpu, memRequirements, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    
    VK_CHECK(vkAllocateMemory(device, &allocInfo, 0, &image.memory));
    VK_CHECK(vkBindImageMemory(device, image.image, image.memory, 0));
    
    return image;
}

VkBufferImageCopy create_copy_region(VkDeviceSize buffer_offset, u32 buffer_row_length, u32 buffer_image_height, VkImageSubresourceLayers image_subresource, VkOffset3D image_offset, VkExtent3D image_extent){
    
    VkBufferImageCopy copy_region;
    
    copy_region.bufferOffset = buffer_offset;
    copy_region.bufferRowLength = buffer_row_length;
    copy_region.bufferImageHeight = buffer_image_height;
    copy_region.imageSubresource = image_subresource;
    copy_region.imageOffset  = image_offset;
    copy_region.imageExtent = image_extent;
    
    return copy_region;
    
};