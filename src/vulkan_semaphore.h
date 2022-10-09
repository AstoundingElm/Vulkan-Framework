#pragma once

#include "defines.h"
#include "vulkan_functions.h"
#include "vulkan_types.h"

PINLINE VkFenceCreateInfo create_fence_info(const void * pnext,VkFenceCreateFlags flags) {
    VkFenceCreateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    info.pNext = pnext;
    info.flags = flags;
    return info;
};

PINLINE VkSemaphoreCreateInfo create_semaphore_info( const void * semaphore_pnext, VkSemaphoreCreateFlags semaphore_flags){
    
    VkSemaphoreCreateInfo semaphore_create_info = {};
    semaphore_create_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    semaphore_create_info.pNext = semaphore_pnext;
    semaphore_create_info.flags = semaphore_flags;
    
    return semaphore_create_info;
    
};

PINLINE VkResult create_semaphore_objects(logical_device_types * logical_context, semaphore_types * semaphore_context){
    
    VkSemaphoreCreateInfo semaphore_info = create_semaphore_info(NULL, 0);
    VK_CHECK(vkCreateSemaphore(logical_context->logical_device, &semaphore_info, 0, &semaphore_context->acquire_semaphore));
    
    VK_CHECK(vkCreateSemaphore(logical_context->logical_device, &semaphore_info, 0, &semaphore_context->submit_semaphore));
    
    VkFenceCreateInfo fence_info = create_fence_info(NULL, VK_FENCE_CREATE_SIGNALED_BIT);
    VK_CHECK(vkCreateFence(logical_context->logical_device, &fence_info, 0, &semaphore_context->image_available_fences));
    return VK_SUCCESS;
};