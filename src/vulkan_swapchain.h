#pragma once

#include "defines.h"
#include "vulkan_functions.h"
#include "vulkan_types.h"
#include <stdio.h>

PINLINE VkSwapchainCreateInfoKHR create_swap_info(VkSwapchainCreateFlagsKHR swapchain_flags, const void * swapchain_info_pnext,VkSurfaceKHR surface, u32 min_image_count, VkFormat image_format,
                                                  VkColorSpaceKHR image_color_space, VkExtent2D image_extent,u32 image_array_layers, VkImageUsageFlags image_usage, VkSharingMode image_sharing_mode,
                                                  u32 queue_family_index_count, const u32 *queue_family_indices,
                                                  VkSurfaceTransformFlagBitsKHR pretransform, VkCompositeAlphaFlagBitsKHR composite_alpha, VkPresentModeKHR present_mode,
                                                  VkBool32 b_clipped, VkSwapchainKHR old_swapchain) {
    
    VkSwapchainCreateInfoKHR swap_info_create = {};
    swap_info_create.sType  = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
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

PINLINE VkResult create_swapchain(instance_types * instance_context,
                                  physical_device_types * physical_context,
                                  surface_types * surface_context, logical_device_types * logical_context,
                                  swapchain_types * swapchain_context
                                  ){
    
    
    VkSurfaceFormatKHR surface_formats[10];
    
    VK_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(physical_context->physical_device_best_device,
                                                  surface_context->surface, &surface_context->surface_format_count, 0));
    VK_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(physical_context->physical_device_best_device, surface_context->surface, &surface_context->surface_format_count,  
                                                  surface_formats));
    
    for(u32 i = 0; i < surface_context->surface_format_count; i++){
        
        VkSurfaceFormatKHR format = surface_formats[i];
        if(format.format == VK_FORMAT_B8G8R8A8_SRGB){
            surface_context->surface_formats = format;
            break;
        }
        
    }
    
    VK_CHECK(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physical_context->physical_device_best_device, surface_context->surface, &surface_context->surface_capabilities));
    u32 image_count = surface_context->surface_capabilities.minImageCount + 1;
    image_count = image_count > surface_context->surface_capabilities.maxImageCount ? image_count -1 : image_count;
    
    VkSwapchainCreateInfoKHR swap_info = create_swap_info(0, NULL, surface_context->surface, image_count, surface_context->surface_formats.format, surface_context->surface_formats.colorSpace, surface_context->surface_capabilities.currentExtent,
                                                          1, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, VK_SHARING_MODE_EXCLUSIVE, 2, NULL,
                                                          surface_context->surface_capabilities.currentTransform,
                                                          VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR, VK_PRESENT_MODE_MAILBOX_KHR, VK_TRUE, VK_NULL_HANDLE);
    /* swap_types->swapchain_info = create_swap_info(0, NULL, vulkan_context->surface, surface_context->surface_capabilities.minImageCount + 1, 
    surface_context->surface_formats[0].format, surface_context->surface_formats[0].colorSpace, extent, 1,
     VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, image_sharing_mode, 2 , queue_family_indices, surface_context->surface_capabilities.currentTransform, VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR, 
    surface_context-> mailbox_mode_supported ? VK_PRESENT_MODE_MAILBOX_KHR : VK_PRESENT_MODE_FIFO_KHR, VK_TRUE, VK_NULL_HANDLE);*/
    
    /* VkSwapchainCreateInfoKHR scInfo = {};
        scInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        scInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        scInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        scInfo.surface = vkcontext->surface;
        scInfo.imageFormat = vkcontext->surfaceFormat.format;
        scInfo.preTransform = surfaceCaps.currentTransform;
        scInfo.imageExtent = surfaceCaps.currentExtent;
        scInfo.minImageCount = imgCount;
        scInfo.imageArrayLayers = 1;*/
    VK_CHECK(vkCreateSwapchainKHR(logical_context->logical_device, &swap_info, instance_context->allocator, &swapchain_context->swapchain));
    
    VK_CHECK(vkGetSwapchainImagesKHR(logical_context->logical_device, swapchain_context->swapchain, &swapchain_context->swapchain_image_count, 0));
    
    
    VK_CHECK(vkGetSwapchainImagesKHR(logical_context->logical_device, swapchain_context->swapchain, &swapchain_context->swapchain_image_count, swapchain_context->swapchain_images));
    VkImageViewCreateInfo viewInfo = {};
    viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.format = surface_context->surface_formats.format;
    viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    viewInfo.subresourceRange.levelCount = 1;
    viewInfo.subresourceRange.layerCount = 1;
    for (u32 i = 0; i < swapchain_context->swapchain_image_count; i++)
    {
        viewInfo.image = swapchain_context->swapchain_images[i];
        VK_CHECK(vkCreateImageView(logical_context->logical_device, &viewInfo, 0, &swapchain_context->swapchain_image_views[i]));
    }
    return VK_SUCCESS;
};

PINLINE VkFramebufferCreateInfo create_framebuffer_info(const void * pnext, VkRenderPass render_pass, u32 attachment_count, const VkImageView * pattachments, 
                                                        u32 ren_pass_width, u32 ren_pass_height,
                                                        u32 ren_pass_layers) {
    VkFramebufferCreateInfo framebuffer_info = {};
    framebuffer_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebuffer_info.pNext = pnext;
    framebuffer_info.flags = 0;
    framebuffer_info.renderPass = render_pass;
    framebuffer_info.attachmentCount = attachment_count;
    framebuffer_info.pAttachments = pattachments;
    framebuffer_info.width = ren_pass_width;
    framebuffer_info.height = ren_pass_height;
    framebuffer_info.layers = ren_pass_layers;
    
    return framebuffer_info;
};

PINLINE VkViewport create_viewport(float viewport_x, float viewport_y , float viewport_width, float viewport_height, float viewport_min_depth, float viewport_max_depth){
    
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