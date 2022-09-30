#pragma once

#include "vulkan_types.h"
#include "vulkan_functions.h"
#include "defines.h"
#include "buffer_types.h"
#include "entity.h"


VkSubmitInfo create_submit_info(const void * submit_info_pnext,u32 submit_info_wait_semaphore_count, const VkSemaphore * submit_info_wait_semaphores, 
                                const VkPipelineStageFlags * wait_dst_stage_mask, u32 command_buffer_count, const VkCommandBuffer * pointer_command_buffers, u32 signal_semaphore_count,
                                const VkSemaphore * submit_info_signal_semaphores ){
    
    VkSubmitInfo submit_info;
    submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
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
    
    VkPresentInfoKHR present_info = {};
    present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    present_info.pNext = present_pnext;
    present_info.waitSemaphoreCount = present_info_wait_semaphore_count;
    present_info.pWaitSemaphores = present_info_wait_semaphores;
    present_info.swapchainCount = present_info_swapchain_count;
    present_info.pSwapchains = present_info_swapchains;
    present_info.pImageIndices = present_info_image_indices;
    present_info.pResults = present_info_results;
    
    return present_info;
    
};

VkCommandBufferBeginInfo cmd_begin_info()
{
    VkCommandBufferBeginInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    
    return info;
}

VkSubmitInfo submit_info(VkCommandBuffer* cmd, uint32_t cmdCount = 1)
{
    VkSubmitInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    info.commandBufferCount = cmdCount;
    info.pCommandBuffers = cmd;
    
    return info;
}

PINLINE VkResult render_loop(logical_device_types * logical_context, semaphore_types * semaphore_context, command_pool_types * command_context, swapchain_types * swapchain_context, queue_types * queue_context,
                             renderpass_types * renderpass_context,
                             framebuffer_types * framebuffer_context,
                             pipeline_types * pipeline_context,
                             descriptor_set_types * set_context,
                             screen_types * screen_context, Game_State * game_state){
    
    u32 image_index;
    
    VK_CHECK(vkWaitForFences(logical_context->logical_device,1, &semaphore_context->image_available_fences, VK_TRUE, UINT64_MAX));
    
    VK_CHECK(vkResetFences(logical_context->logical_device, 1, &semaphore_context->image_available_fences));
    
    vk_copy_to_buffer(&transform_storage_buffer,&game_state->entities, sizeof(Transform) *game_state->entity_count);
    
    
    VK_CHECK(vkAcquireNextImageKHR(logical_context->logical_device, swapchain_context->swapchain, UINT64_MAX, semaphore_context-> acquire_semaphore, 0, &image_index));
    VkCommandBuffer cmd = command_context->command_buffer;
    vkResetCommandBuffer(cmd, 0);
    VkCommandBufferBeginInfo begin_info = cmd_begin_info();
    VK_CHECK(vkBeginCommandBuffer(cmd, &begin_info));
    
    VkClearValue clear_value = {};
    clear_value.color = {1, 1, 0, 1};
    
    VkRenderPassBeginInfo renderpass_begin_info = {};
    
    renderpass_begin_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderpass_begin_info.renderArea.extent  = screen_context->screen_size;
    renderpass_begin_info.clearValueCount = 1;
    renderpass_begin_info.pClearValues = &clear_value;
    renderpass_begin_info.renderPass = renderpass_context->renderpass;
    renderpass_begin_info.framebuffer = framebuffer_context->framebuffers[image_index];
    vkCmdBeginRenderPass(cmd, &renderpass_begin_info, VK_SUBPASS_CONTENTS_INLINE);
    
    
    VkViewport viewport = {};
    viewport.maxDepth = 1.0f;
    viewport.width = screen_context->screen_size.width;
    viewport.height = screen_context->screen_size.height;
    
    VkRect2D scissor = {};
    scissor.extent = screen_context->screen_size;
    
    vkCmdSetViewport(cmd, 0, 1, &viewport);
    vkCmdSetScissor(cmd, 0, 1, &scissor);
    
    vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline_context->pipeline_layout, 0, 1, &set_context->descriptor_set, 0, 0);
    
    vkCmdBindIndexBuffer(cmd, indexBuffer.buffer, 0, VK_INDEX_TYPE_UINT32);
    vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline_context->pipeline);
    vkCmdDrawIndexed(cmd, 6, game_state->entity_count, 0, 0, 0);
    
    vkCmdEndRenderPass(cmd);
    VK_CHECK(vkEndCommandBuffer(cmd));
    
    VkPipelineStageFlags wait_stage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    
    VkSubmitInfo vk_submit_info = submit_info(&cmd);
    
    vk_submit_info.pWaitDstStageMask = &wait_stage;
    vk_submit_info.pSignalSemaphores = &semaphore_context->submit_semaphore;
    vk_submit_info.signalSemaphoreCount = 1;
    vk_submit_info.pWaitSemaphores = &semaphore_context->acquire_semaphore;
    vk_submit_info.waitSemaphoreCount = 1;
    VK_CHECK(vkQueueSubmit(queue_context->graphics_queue, 1, &vk_submit_info, semaphore_context->image_available_fences));
    
    VkPresentInfoKHR present_info = {};
    present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    present_info.pSwapchains = &swapchain_context->swapchain;
    present_info.swapchainCount  = 1;
    present_info.pImageIndices = &image_index;
    present_info.pWaitSemaphores = &semaphore_context->submit_semaphore;
    present_info.waitSemaphoreCount = 1;
    vkQueuePresentKHR(queue_context->graphics_queue, &present_info);
    
    return VK_SUCCESS;
    
    
};