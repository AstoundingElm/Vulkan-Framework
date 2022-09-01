#pragma once

#include "vulkan_types.h"
#include "vulkan_functions.h"
#include "defines.h"


VkSubmitInfo create_submit_info(const void * submit_info_pnext,u32 submit_info_wait_semaphore_count, const VkSemaphore * submit_info_wait_semaphores, 
const VkPipelineStageFlags * wait_dst_stage_mask, u32 command_buffer_count, const VkCommandBuffer * pointer_command_buffers, u32 signal_semaphore_count,
const VkSemaphore * submit_info_signal_semaphores ){

    VkSubmitInfo submit_info = {VK_STRUCTURE_TYPE_SUBMIT_INFO};
    submit_info.pNext = submit_info_pnext;
    submit_info.waitSemaphoreCount = submit_info_wait_semaphore_count;
    submit_info.pWaitSemaphores = submit_info_wait_semaphores;
    submit_info.pWaitDstStageMask = wait_dst_stage_mask;
    submit_info.commandBufferCount = command_buffer_count;
    submit_info.pCommandBuffers = pointer_command_buffers;
    submit_info.signalSemaphoreCount = signal_semaphore_count;
    submit_info.pSignalSemaphores = submit_info_signal_semaphores;

    return submit_info;
}

VkPresentInfoKHR create_present_info(const void * present_pnext, u32 present_info_wait_semaphore_count, const VkSemaphore * present_info_wait_semaphores, 
u32 present_info_swapchain_count, const VkSwapchainKHR * present_info_swapchains, const u32 * present_info_image_indices, VkResult * present_info_results){

    VkPresentInfoKHR present_info = {VK_STRUCTURE_TYPE_PRESENT_INFO_KHR};
    present_info.pNext = present_pnext;
    present_info.waitSemaphoreCount = present_info_wait_semaphore_count;
    present_info.pWaitSemaphores = present_info_wait_semaphores;
    present_info.swapchainCount = present_info_swapchain_count;
    present_info.pSwapchains = present_info_swapchains;
    present_info.pImageIndices = present_info_image_indices;
    present_info.pResults = present_info_results;

    return present_info;

};