#include "vulkan_swapchain.h"

#include "core/logger.h"
#include "core/kmemory.h"
#include "vulkan_device.h"
#include "vulkan_image.h"
VkSwapchainCreateInfoKHR create_swap_info(VkSwapchainCreateFlagsKHR swapchain_flags, const void * swapchain_info_pnext,VkSurfaceKHR surface, u32 min_image_count, VkFormat image_format,
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

void create(vulkan_context* context, u32 width, u32 height, vulkan_swapchain* swapchain){
        
        VkExtent2D swapchain_extent = {1000, 800};
        
        VkSurfaceFormatKHR surface_formats[10];
        
        VK_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(context->device.physical_device,
                                                      context->surface, &context->swapchain_support.format_count, 0));
        VK_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(context->device.physical_device, context->surface, &context->swapchain_support.format_count,  
                                                      surface_formats));
        
        for(u32 i = 0; i < context->swapchain_support.format_count; i++){
                
                VkSurfaceFormatKHR format = surface_formats[i];
                if(format.format == VK_FORMAT_B8G8R8A8_SRGB){
                        swapchain->image_format = format;
                        break;
                }
                
        }
        
        
        
        VK_CHECK(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(context->device.physical_device,context->surface, &context->swapchain_support.capabilities));
        u32 image_count = context->swapchain_support.capabilities.minImageCount + 1;
        image_count = image_count > context->swapchain_support.capabilities.maxImageCount ? image_count -1 : image_count;
        
        VkSwapchainCreateInfoKHR   swap_info = create_swap_info(0, NULL, context->surface, image_count, swapchain->image_format.format, swapchain->image_format.colorSpace, context->swapchain_support.capabilities.currentExtent,
                                                                1, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, VK_SHARING_MODE_EXCLUSIVE, 2, NULL,
                                                                context->swapchain_support.capabilities.currentTransform,
                                                                VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR, VK_PRESENT_MODE_MAILBOX_KHR, VK_TRUE, VK_NULL_HANDLE);
        
        
        VK_CHECK(vkCreateSwapchainKHR(context->device.logical_device, &swap_info, context->allocator, &swapchain->handle));
        
        
        context->current_frame = 0;
        swapchain->image_count = 0;
        
        VK_CHECK(vkGetSwapchainImagesKHR(context->device.logical_device, swapchain->handle, &swapchain->image_count, 0));
        if (!swapchain->images) {
                swapchain->images = (VkImage*)kallocate(sizeof(VkImage) * swapchain->image_count, MEMORY_TAG_RENDERER);
        }
        if (!swapchain->views) {
                swapchain->views = (VkImageView*)kallocate(sizeof(VkImageView) * swapchain->image_count, MEMORY_TAG_RENDERER);
        }
        
        
        
        VK_CHECK(vkGetSwapchainImagesKHR(context->device.logical_device, swapchain->handle, &swapchain->image_count, swapchain->images));
        
        VkImageViewCreateInfo viewInfo = {};
        viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        viewInfo.format = swapchain->image_format.format;
        viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        viewInfo.subresourceRange.levelCount = 1;
        viewInfo.subresourceRange.layerCount = 1;
        for (u32 i = 0; i < swapchain->image_count; i++)
        {
                viewInfo.image = swapchain->images[i];
                VK_CHECK(vkCreateImageView(context->device.logical_device, &viewInfo, 0, &swapchain->views[i]));
        }
        
        if(!vulkan_device_detect_depth_format(&context->device)){
                
                context->device.depth_format = VK_FORMAT_UNDEFINED;
                KFATAL("Failed to find a supported format");
                
                
        }
        
        vulkan_image_create(context, 
                            VK_IMAGE_TYPE_2D, swapchain_extent.width, swapchain_extent.height, context->device.depth_format, 
                            VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                            true, VK_IMAGE_ASPECT_DEPTH_BIT,  &swapchain->depth_attachment );
        
        
        KINFO("Swpachain created successfully");
        
};

void destroy(vulkan_context* context, vulkan_swapchain* swapchain){
        
        vkDeviceWaitIdle(context->device.logical_device);
        
        vulkan_image_destroy(context, &swapchain->depth_attachment);
        
        // Only destroy the views, not the images, since those are owned by the swapchain and are thus
        // destroyed when it is.
        for (u32 i = 0; i < swapchain->image_count; ++i) {
                vkDestroyImageView(context->device.logical_device, swapchain->views[i], context->allocator);
        }
        
        vkDestroySwapchainKHR(context->device.logical_device, swapchain->handle, context->allocator);
        
        
};

void vulkan_swapchain_create(vulkan_context * context, u32 width, u32 height, vulkan_swapchain * out_swapchain){
        
        create(context, width, height, out_swapchain);
        
};

void vulkan_swapchain_recreate(vulkan_context * context, u32 width, u32 height, vulkan_swapchain * swapchain){
        
        destroy(context, swapchain);
        create(context, width, height, swapchain);
};

void vulkan_swapchain_destroy(vulkan_context * context,  vulkan_swapchain * swapchain){
        
        destroy(context, swapchain);
};

b8 vulkan_swapchain_acquire_next_image_index(
                                             vulkan_context* context,
                                             vulkan_swapchain* swapchain,
                                             u64 timeout_ns,
                                             VkSemaphore image_available_semaphore,
                                             VkFence fence,
                                             u32* out_image_index){
        
        VkResult result = vkAcquireNextImageKHR(context->device.logical_device, swapchain->handle, timeout_ns, image_available_semaphore, fence, out_image_index);
        
        if (result == VK_ERROR_OUT_OF_DATE_KHR) {
                // Trigger swapchain recreation, then boot out of the render loop.
                vulkan_swapchain_recreate(context, context->framebuffer_width, context->framebuffer_height, swapchain);
                return false;
        } else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
                KFATAL("Failed to acquire swapchain image!");
                return false;
        }
        
        return true;
};


void vulkan_swapchain_present(
                              vulkan_context* context,
                              vulkan_swapchain* swapchain,
                              VkQueue graphics_queue,
                              VkSemaphore render_complete_semaphore,
                              u32 present_image_index) {
        // Return the image to the swapchain for presentation.
        VkPresentInfoKHR present_info = {VK_STRUCTURE_TYPE_PRESENT_INFO_KHR};
        present_info.waitSemaphoreCount = 1;
        present_info.pWaitSemaphores = &render_complete_semaphore;
        present_info.swapchainCount = 1;
        present_info.pSwapchains = &swapchain->handle;
        present_info.pImageIndices = &present_image_index;
        present_info.pResults = 0;
        
        VkResult result = vkQueuePresentKHR(graphics_queue, &present_info);
        if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
                // Swapchain is out of date, suboptimal or a framebuffer resize has occurred. Trigger swapchain recreation.
                vulkan_swapchain_recreate(context, context->framebuffer_width, context->framebuffer_height, swapchain);
        } else if (result != VK_SUCCESS) {
                KFATAL("Failed to present swap chain image!");
        }
        
        // Increment (and loop) the index.
        context->current_frame = (context->current_frame + 1) % swapchain->max_frames_in_flight;
}
