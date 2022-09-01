#include "vulkan_functions.h"
#include "defines.h"
#include "platform.h"
#include "vulkan_device.h"
#include "vulkan_instance.h"
#include "vulkan_queues.h"
#include "shader_utils.h"
#include "vulkan_pipeline.h"
#include "command_buffers.h"
#include "vulkan_semaphore.h"
#include "vulkan_renderpass.h"
#include "vulkan_swapchain.h"
#include "vulkan_utils.h"
#include "vulkan_images.h"
#include "vulkan_surface.h"
#include "render_loop.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <dlfcn.h>
#include <string.h>

#define  MAX_FRAMES  2

PINLINE void cleanup_swapchain(){


}

PINLINE void recreate_swapchain(vulkan_types * vk_context){

    vkDeviceWaitIdle(vk_context->logical_device);
    cleanup_swapchain();
   // create_swapchain();
   // create_image_view();
   // create_framebuffers();

}

PINLINE VkResult run(vulkan_types * vulkan_context, physical_device_types * device_types, queue_types * queue_context, surface_types * surface_cont) {

   VK_CHECK(create_instance(&vk_context));
   
   VK_CHECK(pick_physical_device(&vk_context, &device_context));

   get_queue_families(&vk_context, &q_context, &device_context);

    init_xwindow();

    create_surface(&vk_context, &device_context, &q_context, &surface_context);
   
    u32 queue_family_indices[2] = {0, 1};

 VkSwapchainCreateInfoKHR swap_info = create_swap_info(0, NULL, vulkan_context->surface, surface_cont->surface_capabilities.minImageCount + 1, surface_cont->surface_formats[0].format,
surface_cont->surface_formats[0].colorSpace,surface_cont->extent_suitable ? surface_cont->surface_capabilities.currentExtent : surface_cont->actual_extent,   1, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, 
queue_context->single_queue ? VK_SHARING_MODE_EXCLUSIVE : VK_SHARING_MODE_CONCURRENT, queue_context->single_queue ? 0 : 2, 
 queue_context->single_queue ? NULL : queue_family_indices, 
surface_cont->surface_capabilities.currentTransform, VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,surface_cont-> mailbox_mode_supported ? VK_PRESENT_MODE_MAILBOX_KHR : VK_PRESENT_MODE_FIFO_KHR, 
VK_TRUE, VK_NULL_HANDLE);

    VkSwapchainKHR swap;
    VK_CHECK(vkCreateSwapchainKHR(vulkan_context->logical_device, &swap_info, vulkan_context->allocator, &swap));

    u32 swap_image_count;
    vkGetSwapchainImagesKHR(vulkan_context->logical_device, swap, &swap_image_count, NULL);
    VkImage swap_images[swap_image_count];
    vkGetSwapchainImagesKHR(vulkan_context->logical_device, swap, &swap_image_count, swap_images);

    VkImageView image_views[swap_image_count];
    VkImageViewCreateInfo image_view_create_info[swap_image_count];
    VkComponentMapping image_view_rba_comp = create_component_mapping(VK_COMPONENT_SWIZZLE_R , VK_COMPONENT_SWIZZLE_G, 
    VK_COMPONENT_SWIZZLE_B, VK_COMPONENT_SWIZZLE_A);
 
    VkImageSubresourceRange image_subresource_range = create_image_subresource_range(VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, swap_info.imageArrayLayers );

    for (u32 i = 0; i < swap_image_count; i++) {
        image_view_create_info[i].sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        image_view_create_info[i].pNext = NULL;
        image_view_create_info[i].flags = 0;
        image_view_create_info[i].image = swap_images[i];
        image_view_create_info[i].viewType = VK_IMAGE_VIEW_TYPE_2D;
        image_view_create_info[i].format = surface_cont->surface_formats[0].format;
        image_view_create_info[i].components = image_view_rba_comp;
        image_view_create_info[i].subresourceRange = image_subresource_range;
        VK_CHECK(vkCreateImageView(vulkan_context->logical_device, &image_view_create_info[i], NULL, &image_views[i]));
    };

    VkAttachmentDescription attachment_description = create_attachment_description(0, swap_info.imageFormat, VK_SAMPLE_COUNT_1_BIT, VK_ATTACHMENT_LOAD_OP_CLEAR,
     VK_ATTACHMENT_STORE_OP_STORE, VK_ATTACHMENT_LOAD_OP_DONT_CARE, VK_ATTACHMENT_STORE_OP_STORE, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);

    VkAttachmentReference attachment_reference = create_attachment_reference(0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

    VkSubpassDescription subpass_description = create_subpass_desc(0, VK_PIPELINE_BIND_POINT_GRAPHICS, 0, NULL, 1, &attachment_reference, NULL, NULL, 0, NULL);

    VkSubpassDependency subpass_dependency = create_subpass_dependency(VK_SUBPASS_EXTERNAL, 0, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, 
    VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,0, VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT, 0);

    VkRenderPassCreateInfo renderpass_info = create_renderpass_info(NULL, 0, 1, &attachment_description, 1, &subpass_description, 1, &subpass_dependency);

    VK_CHECK(vkCreateRenderPass(vulkan_context->logical_device, &renderpass_info, vulkan_context->allocator, &vulkan_context->renderpass));

    read_shader_file();

    VkShaderModuleCreateInfo frag_module_create_info = create_shader_module_info(NULL, 0, file_info.shader_loaded ? file_info.frag_size : 0, 
    file_info.shader_loaded ? (const u32 *)file_info.frag_code : NULL);

    VkShaderModuleCreateInfo vert_module_create_info = create_shader_module_info(NULL, 0, file_info.shader_loaded ? file_info.vert_size : 0, 
    file_info.shader_loaded ? (const u32 *)file_info.vert_code : NULL);

VkShaderModule vert_shader_module, frag_shader_module;
VK_CHECK(vkCreateShaderModule(vulkan_context->logical_device, &vert_module_create_info, vulkan_context->allocator, &vert_shader_module));
VK_CHECK(vkCreateShaderModule(vulkan_context->logical_device, &frag_module_create_info, vulkan_context->allocator, &frag_shader_module));

VkPipelineShaderStageCreateInfo vert_stage_create_info = create_pipeline_shader_stage_info(NULL, 0, VK_SHADER_STAGE_VERTEX_BIT,vert_shader_module, "main", NULL);

VkPipelineShaderStageCreateInfo frag_stage_create_info = create_pipeline_shader_stage_info(NULL, 0, VK_SHADER_STAGE_FRAGMENT_BIT, frag_shader_module, "main", NULL);
VkPipelineShaderStageCreateInfo   shader_stage_create_infos[2];

shader_stage_create_infos[0] = vert_stage_create_info;
shader_stage_create_infos[1] = frag_stage_create_info;

VkPipelineVertexInputStateCreateInfo vertex_input_info = create_vertex_input_state(0, NULL, 0, NULL);

VkPipelineInputAssemblyStateCreateInfo input_assembly_state_create_info = create_input_assembly(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);

VkViewport viewport = create_viewport(0.0f, 0.0f, swap_info.imageExtent.width, swap_info.imageExtent.height, 0.0f, 1.0f);

VkOffset2D scissor_offset = {0, 0};
VkRect2D scissor =  {scissor_offset, swap_info.imageExtent};

VkPipelineViewportStateCreateInfo viewport_state_info = create_viewport_state_info(1, &viewport, 1, &scissor);

VkPipelineRasterizationStateCreateInfo rasterization_state_create_info = create_rasterization_state_info(VK_FALSE, VK_FALSE, VK_POLYGON_MODE_FILL, 
VK_CULL_MODE_NONE, VK_FRONT_FACE_CLOCKWISE, VK_FALSE, 0.0f, 0.0f, 0.0f, 1.0f);

VkPipelineMultisampleStateCreateInfo multisample_state_create_info = create_multisample_state_info(VK_SAMPLE_COUNT_1_BIT, VK_FALSE, 1.0f, NULL, VK_FALSE, 
VK_FALSE);

VkPipelineColorBlendAttachmentState color_blend_attachment_state = create_color_blend_attach_state(VK_FALSE, VK_BLEND_FACTOR_ONE, VK_BLEND_FACTOR_ZERO, 
VK_BLEND_OP_ADD, VK_BLEND_FACTOR_ONE, VK_BLEND_FACTOR_ZERO, VK_BLEND_OP_ADD, VK_COLOR_COMPONENT_R_BIT|VK_COLOR_COMPONENT_G_BIT
|VK_COLOR_COMPONENT_B_BIT|VK_COLOR_COMPONENT_A_BIT);

 VkPipelineColorBlendStateCreateInfo colour_blend_state_info = create_colour_blend_state(VK_FALSE, VK_LOGIC_OP_COPY, 1, &color_blend_attachment_state);

VkPipelineLayoutCreateInfo pipeline_layout_create_info = create_pipeline_layout_info(0, NULL, 0, NULL);

VkPipelineLayout pipeline_layout;
VK_CHECK(vkCreatePipelineLayout(vulkan_context->logical_device, &pipeline_layout_create_info, vulkan_context->allocator, &pipeline_layout));

VkGraphicsPipelineCreateInfo graphics_pipeline_create_info = create_graphics_pipeline_info(2, shader_stage_create_infos, &vertex_input_info, 
&input_assembly_state_create_info, NULL, &viewport_state_info, &rasterization_state_create_info, &multisample_state_create_info, 
NULL, &colour_blend_state_info, 
NULL, pipeline_layout, vulkan_context->renderpass, 0, VK_NULL_HANDLE, -1 );
VkPipeline pipeline;
VK_CHECK(vkCreateGraphicsPipelines(vulkan_context->logical_device, VK_NULL_HANDLE, 1, &graphics_pipeline_create_info, vulkan_context->allocator, &pipeline));

vkDestroyShaderModule(vulkan_context->logical_device, frag_shader_module, vulkan_context->allocator);
vkDestroyShaderModule(vulkan_context->logical_device, vert_shader_module, vulkan_context->allocator);
free(file_info.vert_code);
free(file_info.frag_code);

VkFramebufferCreateInfo framebuffer_create_info[swap_image_count];
VkFramebuffer frame_buffers[swap_image_count];
VkImageView image_attachments[swap_image_count];
for(u32 i = 0; i < swap_image_count; i++){

    image_attachments[i] = image_views[i];
    framebuffer_create_info[i] = create_framebuffer_info(vulkan_context->renderpass, 1, &(image_attachments[i]), swap_info.imageExtent.width, 
    swap_info.imageExtent.height, 1);
    VK_CHECK(vkCreateFramebuffer(vulkan_context->logical_device, &(framebuffer_create_info[i]), vulkan_context->allocator, &(frame_buffers[i])));
};

VkCommandPoolCreateInfo command_pool_create_info = create_command_pool_info( queue_context->queue_family_best_queue_index);

VkCommandPool command_pool;
VK_CHECK(vkCreateCommandPool(vulkan_context->logical_device, &command_pool_create_info, NULL, &command_pool));

VkCommandBufferAllocateInfo command_buffer_alloc_info = cmd_alloc_info(command_pool, VK_COMMAND_BUFFER_LEVEL_PRIMARY, swap_image_count);

VkCommandBuffer command_buffers[swap_image_count];
VK_CHECK(vkAllocateCommandBuffers(vulkan_context->logical_device, &command_buffer_alloc_info, command_buffers));

//VkCommandBufferBeginInfo command_buffer_begin_info[swap_image_count] = cmd_begin_info()
VkCommandBufferBeginInfo command_buffer_begin_info[swap_image_count];
VkRenderPassBeginInfo renderpass_begin_info[swap_image_count];
VkRect2D renderpass_area;
renderpass_area.offset.x = 0;
renderpass_area.offset.y = 0;
renderpass_area.extent = swap_info.imageExtent;
VkClearValue clear_val = {{{0.6f, 0.2f, 0.8f , 0.0f}}};
for(u32 i = 0; i < swap_image_count; i++){

    command_buffer_begin_info[i].sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    command_buffer_begin_info[i].pNext = NULL;
    command_buffer_begin_info[i].flags = 0;
     command_buffer_begin_info[i].pInheritanceInfo =NULL;
     
     renderpass_begin_info[i].sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
     renderpass_begin_info[i].pNext = NULL;
     renderpass_begin_info[i].renderPass = vulkan_context->renderpass;
     renderpass_begin_info[i].framebuffer = frame_buffers[i];
     renderpass_begin_info[i].renderArea = renderpass_area;
     renderpass_begin_info[i].clearValueCount = 1;
     renderpass_begin_info[i].pClearValues = &clear_val;

     vkBeginCommandBuffer(command_buffers[i], &command_buffer_begin_info[i]);
     vkCmdBeginRenderPass(command_buffers[i], &(renderpass_begin_info[i]), VK_SUBPASS_CONTENTS_INLINE);

     vkCmdBindPipeline(command_buffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);

		vkCmdDraw(command_buffers[i],3,1,0,0);
     vkCmdEndRenderPass(command_buffers[i]);
     vkEndCommandBuffer(command_buffers[i]);    
}


VkSemaphore semaphore_image_available[MAX_FRAMES];
VkSemaphore semaphore_render_final[MAX_FRAMES];
VkFence fence[MAX_FRAMES];

VkSemaphoreCreateInfo semaphore_create_info = create_semaphore_info(VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO, NULL, 0);

VkFenceCreateInfo fence_create_info = create_fence_info(VK_STRUCTURE_TYPE_FENCE_CREATE_INFO, NULL, VK_FENCE_CREATE_SIGNALED_BIT);
//for(u32 i = 0; i < MAX_FRAMES; i++){
    VK_CHECK(vkCreateSemaphore(vulkan_context->logical_device, &semaphore_create_info, vulkan_context->allocator, &(semaphore_image_available[0])));
   VK_CHECK( vkCreateSemaphore(vulkan_context->logical_device, &semaphore_create_info, vulkan_context->allocator, &(semaphore_render_final[0])));
    VK_CHECK(vkCreateFence(vulkan_context->logical_device, &fence_create_info, vulkan_context->allocator, &(fence[0])));
    
     VK_CHECK(vkCreateSemaphore(vulkan_context->logical_device, &semaphore_create_info, vulkan_context->allocator, &(semaphore_image_available[1])));
   VK_CHECK( vkCreateSemaphore(vulkan_context->logical_device, &semaphore_create_info, vulkan_context->allocator, &(semaphore_render_final[1])));
    VK_CHECK(vkCreateFence(vulkan_context->logical_device, &fence_create_info, vulkan_context->allocator, &(fence[1])));
//};
u32 current_frame = 0;
VkFence image_fence[swap_image_count];
for(u32 i = 0; i < swap_image_count; i++){

    image_fence[i] = VK_NULL_HANDLE;
}
is_running = true;

while(is_running){

 VK_CHECK(vkWaitForFences(vulkan_context->logical_device, 1, &(fence[current_frame]), VK_TRUE, UINT64_MAX));
        u32 image_index = 0;

        VkResult result =  vkAcquireNextImageKHR(vulkan_context->logical_device, swap, UINT64_MAX, semaphore_image_available[current_frame], VK_NULL_HANDLE, &image_index);
        if(result == VK_ERROR_OUT_OF_DATE_KHR ){

            recreate_swapchain();
            return;

        }else if(result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR){

            //return VK_ERROR_OUT_OF_DATE_KHR;
            printf("return false here");
        };

        vkResetFences(vulkan_context->logical_device, 1, &fence[current_frame]);


        if(image_fence[image_index]!= VK_NULL_HANDLE){

            VK_CHECK(vkWaitForFences(vulkan_context->logical_device, 1, &(image_fence[image_index]), VK_TRUE, UINT64_MAX));

        }

        image_fence[image_index] = fence[current_frame];

           VkSemaphore semaphore_wait[1];
        semaphore_wait[0] = semaphore_image_available[current_frame];
        VkPipelineStageFlags wait_stages[1];
        wait_stages[0] = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        
        VkSemaphore signal_semaphore[1];
        signal_semaphore[0] = semaphore_render_final[current_frame];

        VkSubmitInfo submit_info = create_submit_info(NULL, 1, &(semaphore_wait[0]), &(wait_stages[0]), 1, &(command_buffers[0]), 1, &(signal_semaphore[0]) );

        VK_CHECK(vkResetFences(vulkan_context->logical_device, 1, &(fence[current_frame])));
        vkQueueSubmit(queue_context->graphics_queue, 1, &submit_info, fence[current_frame]);

        VkSwapchainKHR swaps[1];
		swaps[0]= swap;

        VkPresentInfoKHR present_info = create_present_info(NULL, 1, &(signal_semaphore[0]),1, &(swaps[0]), &image_index, NULL );

		result = vkQueuePresentKHR(queue_context->present_queue ,&present_info);
if( result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR){

    recreate_swapchain();
}else if(result != VK_SUCCESS){

    printf("return false at end of render loop");
}

		current_frame=(current_frame+1)%MAX_FRAMES;

        xwindow_main_loop();

}

vkDeviceWaitIdle(vulkan_context->logical_device);
vkDestroyDebugUtilsMessengerEXT(vulkan_context->instance, vulkan_context->debug_messenger, vulkan_context->allocator);

vkFreeCommandBuffers(vulkan_context->logical_device, command_pool, swap_image_count, command_buffers);
for(u32 i = 0; i< MAX_FRAMES; i++){

    vkDestroySemaphore(vulkan_context->logical_device, semaphore_render_final[i], vulkan_context->allocator);
     vkDestroySemaphore(vulkan_context->logical_device, semaphore_image_available[i], vulkan_context->allocator);
      vkDestroyFence(vulkan_context->logical_device, fence[i], vulkan_context->allocator);
}

vkDestroyCommandPool(vulkan_context->logical_device, command_pool, vulkan_context->allocator);
for(u32 i = 0; i < swap_image_count; i++){

    vkDestroyFramebuffer(vulkan_context->logical_device, frame_buffers[i], vulkan_context->allocator);
};

vkDestroyPipeline(vulkan_context->logical_device, pipeline, vulkan_context->allocator);
vkDestroyPipelineLayout(vulkan_context->logical_device, pipeline_layout, vulkan_context->allocator);
vkDestroyRenderPass(vulkan_context->logical_device, vulkan_context->renderpass, vulkan_context->allocator);
for(u32 i = 0; i < swap_image_count; i++){

    vkDestroyImageView(vulkan_context->logical_device, image_views[i], vulkan_context->allocator);
}

vkDestroySwapchainKHR(vulkan_context->logical_device, swap, vulkan_context->allocator);
vkDestroySurfaceKHR(vulkan_context->instance, vulkan_context->surface, vulkan_context->allocator);
xcb_destroy_window(xcb_context.connection, xcb_context.window);
vkDestroyDevice(vulkan_context->logical_device, vulkan_context->allocator);

vkDestroyInstance(vulkan_context->instance, vulkan_context->allocator);

dlclose(vulkan_lib);
vulkan_lib = NULL;

return VK_SUCCESS;

};

int main(){
    
VK_CHECK(run(&vk_context, &device_context, &q_context, &surface_context));

return 0;

}

// notes: switch context->allocator, do macro madness for loading functions, = done. maybe mka e afucntion loader with cmjratori switch?
    // and fix device selection.
    // may need to double check stuff like vkimages[5]- are we oing imges? ie check up to video 21, not after
    // double imageviews. double check everything
    // do 600 lines one day, duble check the nexxt/ clean up codebase/make framework;
    // maybe repalce kohi stuff?
    // test thatvalidation layers are woorking- done
    // use renderdoc
    //todo: make assert macro

