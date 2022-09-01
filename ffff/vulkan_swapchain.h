#pragma once

#include "defines.h"
#include "vulkan_functions.h"
#include "vulkan_types.h"

PINLINE VkSwapchainCreateInfoKHR create_swap_info(VkSwapchainCreateFlagsKHR swapchain_flags, const void * swapchain_info_pnext,VkSurfaceKHR surface, u32 min_image_count, VkFormat image_format,
 VkColorSpaceKHR image_color_space, VkExtent2D image_extent,u32 image_array_layers, VkImageUsageFlags image_usage, VkSharingMode image_sharing_mode,
 u32 queue_family_index_count, const u32 *queue_family_indices,
         VkSurfaceTransformFlagBitsKHR pretransform, VkCompositeAlphaFlagBitsKHR composite_alpha, VkPresentModeKHR present_mode,
                         VkBool32 b_clipped, VkSwapchainKHR old_swapchain) {

    VkSwapchainCreateInfoKHR swap_info_create = {VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR};
    swap_info_create.pNext = swapchain_info_pnext;
    swap_info_create.flags = swapchain_flags;
    swap_info_create.surface = surface;
    swap_info_create.minImageCount = min_image_count;
    swap_info_create.imageFormat = image_format;
    swap_info_create.imageColorSpace = image_color_space;
    swap_info_create.imageExtent = image_extent;
    swap_info_create.imageArrayLayers = image_array_layers;
    swap_info_create.imageUsage = image_usage;
    swap_info_create.imageSharingMode = image_sharing_mode;
    swap_info_create.queueFamilyIndexCount = queue_family_index_count;
    swap_info_create.pQueueFamilyIndices = queue_family_indices;
    swap_info_create.preTransform = pretransform;
    swap_info_create.compositeAlpha = composite_alpha;
    swap_info_create.presentMode = present_mode;
    swap_info_create.clipped = b_clipped;
    swap_info_create.oldSwapchain = old_swapchain;

    return swap_info_create;
};

PINLINE VkFramebufferCreateInfo create_framebuffer_info(VkRenderPass render_pass, u32 attachment_count, const VkImageView * pattachments, 
u32 ren_pass_width, u32 ren_pass_height,
                                                u32 ren_pass_layers) {
    VkFramebufferCreateInfo framebuffer_info = {VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO};
    framebuffer_info.pNext = NULL;
    framebuffer_info.flags = 0;
    framebuffer_info.renderPass = render_pass;
    framebuffer_info.attachmentCount = attachment_count;
     framebuffer_info.pAttachments = pattachments;
    framebuffer_info.width = ren_pass_width;
    framebuffer_info.height = ren_pass_height;
    framebuffer_info.layers = ren_pass_layers;

    return framebuffer_info;
};

PINLINE VkViewport create_viewport(float viewport_x, float viewport_y , float viewport_width, float viewport_height, float viewport_min_depth, float viewport_max_height){

    VkViewport viewport;

    viewport.x = viewport_x;
    viewport.y = viewport_y;
    viewport.width  = viewport_width;
    viewport.height = viewport_height;
    viewport.minDepth = viewport_min_depth;
    viewport.maxDepth = viewport_min_depth;

    return viewport;

}

 PINLINE VkComponentMapping  create_component_mapping(VkComponentSwizzle r, VkComponentSwizzle g, VkComponentSwizzle b, VkComponentSwizzle a ){

     VkComponentMapping component_mapping;
    component_mapping.r = r;
    component_mapping.g = g;
    component_mapping.b = b;
    component_mapping.a = a;

    return component_mapping;

  };