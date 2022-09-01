#pragma once

#include "defines.h"
#include "vulkan_functions.h"
#include "vulkan_types.h"


PINLINE VkCommandBufferBeginInfo cmd_begin_info() {
    VkCommandBufferBeginInfo info;
    info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    return info;
};

PINLINE VkCommandBufferAllocateInfo cmd_alloc_info(VkCommandPool pool, VkCommandBufferLevel command_buffer_level, u32 command_buffer_count) {
    VkCommandBufferAllocateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    info.commandPool = pool;
    info.level = command_buffer_level;
  info.commandBufferCount = command_buffer_count;

    return info;
};

PINLINE VkCommandPoolCreateInfo create_command_pool_info( u32 queue_family_index) {
    VkCommandPoolCreateInfo command_pool_info = {VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO};
    command_pool_info.pNext = NULL;
    command_pool_info.flags = 0;
    command_pool_info.queueFamilyIndex = queue_family_index;

    return command_pool_info;
};