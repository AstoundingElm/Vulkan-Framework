#pragma once

#include "defines.h"
#include "vulkan_types.h"
#include "vulkan_functions.h"

PINLINE VkResult create_framebuffers(framebuffer_types * framebuffer_context, 
                                     renderpass_types * render_context, swapchain_types * swapchain_context, logical_device_types * logical_context){
    
    VkFramebufferCreateInfo framebuffer_info = {};
    framebuffer_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebuffer_info.renderPass = render_context->renderpass;
    framebuffer_info.width = screen_context.screen_size.width;
    framebuffer_info.height = screen_context.screen_size.height;
    framebuffer_info.layers = 1;
    framebuffer_info.attachmentCount = 1;
    for(u32 i= 0; i < swapchain_context->swapchain_image_count; i++){
        
        framebuffer_info.pAttachments = &swapchain_context->swapchain_image_views[i];
        VK_CHECK(vkCreateFramebuffer(logical_context->logical_device, &framebuffer_info,0, &framebuffer_context->framebuffers[i]));
        
    };
    
    
    return VK_SUCCESS;
    
}