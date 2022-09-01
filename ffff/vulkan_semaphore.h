#pragma once

#include "defines.h"
#include "vulkan_functions.h"
#include "vulkan_types.h"

PINLINE VkFenceCreateInfo fence_create_info(VkFenceCreateFlags flags = 0) {
    VkFenceCreateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    info.flags = flags;
    return info;
};

VkSemaphoreCreateInfo create_semaphore_info(VkStructureType semaphore_structure_type, const void * semaphore_pnext, VkSemaphoreCreateFlags semaphore_flags){

VkSemaphoreCreateInfo semaphore_create_info;
semaphore_create_info.sType = semaphore_structure_type;
semaphore_create_info.pNext = semaphore_pnext;
semaphore_create_info.flags = semaphore_flags;

return semaphore_create_info;

};

VkFenceCreateInfo create_fence_info(VkStructureType fence_info_structure_type, const void * fence_info_pnext, VkFenceCreateFlags fence_info_flags){

VkFenceCreateInfo fence_info;
fence_info.sType = fence_info_structure_type;
fence_info.pNext = fence_info_pnext;
fence_info.flags = fence_info_flags;

return fence_info;

};