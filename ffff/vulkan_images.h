#pragma once

#include "defines.h"
#include "vulkan_functions.h"
#include "vulkan_types.h"

PINLINE VkImageViewCreateInfo create_image_view_info(VkImage image, VkImageViewType viewtype, VkFormat format) {
    VkImageViewCreateInfo image_view_create_info = {};

    image_view_create_info.sType = {VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO};
    // image_view_create_info.pNext =
    image_view_create_info.image = image;
    image_view_create_info.viewType = viewtype;
    image_view_create_info.format = format;

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