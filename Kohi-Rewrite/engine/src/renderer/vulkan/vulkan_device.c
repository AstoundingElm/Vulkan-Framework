#include "vulkan_device.h"
#include "core/logger.h"
#include "core/kstring.h"
#include "core/kmemory.h"
#include "containers/darray.h"
#include "vulkan_command_buffer.h"
typedef struct vulkan_physical_device_requirements {
        b8 graphics;
        b8 present;
        b8 compute;
        b8 transfer;
        // darray
        const char** device_extension_names;
        b8 sampler_anisotropy;
        b8 discrete_gpu;
} vulkan_physical_device_requirements;

typedef struct vulkan_physical_device_queue_family_info {
        u32 graphics_family_index;
        u32 present_family_index;
        u32 compute_family_index;
        u32 transfer_family_index;
} vulkan_physical_device_queue_family_info;

VkDeviceQueueCreateInfo create_device_queue_info(const void * pnext, VkDeviceQueueCreateFlags queue_flags,u32 queue_family_index, u32 queue_count, const float *queue_priorities) {
        VkDeviceQueueCreateInfo queue_create_info = {};
        queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queue_create_info.pNext = pnext;
        queue_create_info.flags = queue_flags;
        queue_create_info.queueFamilyIndex = queue_family_index;
        queue_create_info.queueCount = queue_count;
        queue_create_info.pQueuePriorities = queue_priorities;
        
        return queue_create_info;
};

void vulkan_device_query_swapchain_support(
                                           VkPhysicalDevice physical_device,
                                           VkSurfaceKHR surface,
                                           vulkan_swapchain_support_info* out_support_info) {
        // Surface capabilities
        VK_CHECK(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
                                                           physical_device,
                                                           surface,
                                                           &out_support_info->capabilities));
        
        // Surface formats
        VK_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(
                                                      physical_device,
                                                      surface,
                                                      &out_support_info->format_count,
                                                      0));
        
        if (out_support_info->format_count != 0) {
                if (!out_support_info->formats) {
                        out_support_info->formats = kallocate(sizeof(VkSurfaceFormatKHR) * out_support_info->format_count, MEMORY_TAG_RENDERER);
                }
                VK_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(
                                                              physical_device,
                                                              surface,
                                                              &out_support_info->format_count,
                                                              out_support_info->formats));
        }
        
        // Present modes
        VK_CHECK(vkGetPhysicalDeviceSurfacePresentModesKHR(
                                                           physical_device,
                                                           surface,
                                                           &out_support_info->present_mode_count,
                                                           0));
        if (out_support_info->present_mode_count != 0) {
                if (!out_support_info->present_modes) {
                        out_support_info->present_modes = kallocate(sizeof(VkPresentModeKHR) * out_support_info->present_mode_count, MEMORY_TAG_RENDERER);
                }
                VK_CHECK(vkGetPhysicalDeviceSurfacePresentModesKHR(
                                                                   physical_device,
                                                                   surface,
                                                                   &out_support_info->present_mode_count,
                                                                   out_support_info->present_modes));
        }
}

b8 vulkan_device_detect_depth_format(vulkan_device * device){
        
        // Format candidates
        const u64 candidate_count = 3;
        VkFormat candidates[3] = {
                VK_FORMAT_D32_SFLOAT,
                VK_FORMAT_D32_SFLOAT_S8_UINT,
                VK_FORMAT_D24_UNORM_S8_UINT};
        
        u32 flags = VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT;
        for (u64 i = 0; i < candidate_count; ++i) {
                VkFormatProperties properties;
                vkGetPhysicalDeviceFormatProperties(device->physical_device, candidates[i], &properties);
                
                if ((properties.linearTilingFeatures & flags) == flags) {
                        device->depth_format = candidates[i];
                        return true;
                } else if ((properties.optimalTilingFeatures & flags) == flags) {
                        device->depth_format = candidates[i];
                        return true;
                }
        }
        
        return false;
        
};

VkDeviceCreateInfo create_device_info(const void * pnext, VkDeviceCreateFlags device_flags, u32 queue_create_info_count, const VkDeviceQueueCreateInfo *queue_create_infos, u32 enabled_extension_count,
                                      const char *const *enabled_extensions_names, const VkPhysicalDeviceFeatures *enabled_features) {
        VkDeviceCreateInfo device_create_info = {};
        device_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        device_create_info.pNext = pnext;
        device_create_info.flags = device_flags;
        device_create_info.queueCreateInfoCount = queue_create_info_count;
        device_create_info.pQueueCreateInfos = queue_create_infos;
        // device_create_info.enabledLayerCount =
        // device_create_info.ppEnabledLayerNames =
        device_create_info.enabledExtensionCount = enabled_extension_count;
        device_create_info.ppEnabledExtensionNames = enabled_extensions_names;
        device_create_info.pEnabledFeatures = enabled_features;
        
        return device_create_info;
};


b8 vulkan_device_create(vulkan_context * context){
        
        KINFO("creating physical device...");
        context->device.graphics_index = -1;
        
        u32 gpu_count = 0;
        
        VkPhysicalDevice gpus[10];
        
        VK_CHECK(vkEnumeratePhysicalDevices(context->instance, &gpu_count, 0));
        VK_CHECK(vkEnumeratePhysicalDevices(context->instance,
                                            &gpu_count, gpus));
        
        
        for(u32 i = 0; i < gpu_count; i++){
                
                
                VkPhysicalDevice gpu = gpus[i];
                u32 queue_family_count = 0;
                
                VkQueueFamilyProperties queue_props[10];
                
                vkGetPhysicalDeviceQueueFamilyProperties(gpu, &queue_family_count, 0);
                vkGetPhysicalDeviceQueueFamilyProperties(gpu, & queue_family_count, queue_props);
                for(u32 j = 0; j < queue_family_count; j++){
                        
                        if(queue_props[j].queueFlags & VK_QUEUE_GRAPHICS_BIT){
                                VkBool32 surface_support = VK_FALSE;
                                
                                VK_CHECK(vkGetPhysicalDeviceSurfaceSupportKHR(gpu, j, context->surface, &surface_support));
                                if(surface_support){
                                        
                                        context->device.graphics_index = j;
                                        context->device.physical_device = gpu;
                                        break;
                                        
                                }
                                
                                
                        }
                        
                }
                
        }
        if(context->device.graphics_index  < 0){
                
                return false;
        }
        KINFO("Creating logical device...");
        
        float queue_priority = 1.0f;
        
        VkDeviceQueueCreateInfo queue_info =  create_device_queue_info(NULL, 0, context->device.graphics_index, 
                                                                       1, &queue_priority);
        
        const char * extensions[] = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};
        VkDeviceCreateInfo device_info = create_device_info(NULL, 0,1, &queue_info,  ArraySize(extensions), extensions, NULL);
        
        VK_CHECK(vkCreateDevice(context->device.physical_device, &device_info, context->allocator, &context->device.logical_device));
        
        KINFO("logical dvice created");
        
        vkGetDeviceQueue(context->device.logical_device, context->device.graphics_index, 0, &context->device.graphics_queue);
        
        KINFO("queues obtained");
        
        // Create command pool for graphics queue.
        VkCommandPoolCreateInfo pool_create_info = {VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO};
        pool_create_info.queueFamilyIndex = context->device.graphics_index;
        pool_create_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        VK_CHECK(vkCreateCommandPool(
                                     context->device.logical_device,
                                     &pool_create_info,
                                     context->allocator,
                                     &context->device.graphics_command_pool));
        KINFO("Graphics command pool created.");
        
        return true;
};

void vulkan_device_destroy(vulkan_context * context){
        
        // Unset queues
        context->device.graphics_queue = 0;
        //context->device.present_queue = 0;
        //context->device.transfer_queue = 0;
        
        KINFO("Destroying command pools...");
        vkDestroyCommandPool(
                             context->device.logical_device,
                             context->device.graphics_command_pool,
                             context->allocator);
        
        // Destroy logical device
        KINFO("Destroying logical device...");
        if (context->device.logical_device) {
                vkDestroyDevice(context->device.logical_device, context->allocator);
                context->device.logical_device = 0;
        }
        
        // Physical devices are not destroyed.
        KINFO("Releasing physical device resources...");
        context->device.physical_device = 0;
        
        if (context->device.swapchain_support.formats) {
                kfree(
                      context->device.swapchain_support.formats,
                      sizeof(VkSurfaceFormatKHR) * context->device.swapchain_support.format_count,
                      MEMORY_TAG_RENDERER);
                context->device.swapchain_support.formats = 0;
                context->device.swapchain_support.format_count = 0;
        }
        
        if (context->device.swapchain_support.present_modes) {
                kfree(
                      context->device.swapchain_support.present_modes,
                      sizeof(VkPresentModeKHR) * context->device.swapchain_support.present_mode_count,
                      MEMORY_TAG_RENDERER);
                context->device.swapchain_support.present_modes = 0;
                context->device.swapchain_support.present_mode_count = 0;
        }
        
        kzero_memory(
                     &context->device.swapchain_support.capabilities,
                     sizeof(context->device.swapchain_support.capabilities));
        
        
}
