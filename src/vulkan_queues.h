#pragma once

#include "defines.h"
#include "vulkan_functions.h"
#include "vulkan_types.h"
#include "vulkan_device.h"
/*
PINLINE VkResult get_queue_families(instance_types * vulkan_context, queue_types * get_queue_context, physical_device_types * device_types,
                                    logical_device_types * logical_device_context){
    
    u32 queue_families_properties_count = 0;
    
    vkGetPhysicalDeviceQueueFamilyProperties(device_types->physical_device_best_device, &queue_families_properties_count, NULL);
    VkQueueFamilyProperties queue_family_properties[queue_families_properties_count];
    vkGetPhysicalDeviceQueueFamilyProperties(device_types->physical_device_best_device, &queue_families_properties_count, queue_family_properties);
    
    u32 queue_family_queue_count[queue_families_properties_count];
    for (u32 i = 0; i < queue_families_properties_count; i++) {
        queue_family_queue_count[i] = queue_family_properties[i].queueCount;
    }
    
    VkDeviceQueueCreateInfo device_queue_create_infos[queue_families_properties_count];
    for (u32 i = 0; i < queue_families_properties_count; i++) {
        device_queue_create_infos[i].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        device_queue_create_infos[i].pNext = NULL;
        device_queue_create_infos[i].flags = 0;
        device_queue_create_infos[i].queueFamilyIndex = i;
        device_queue_create_infos[i].queueCount = queue_family_queue_count[i];
        float queue_priority[1] = {1.0f};
        device_queue_create_infos[i].pQueuePriorities = queue_priority;
    }
    
    printf("using %d queue families\n", queue_families_properties_count);
    
    const char *required_extension = VK_KHR_SWAPCHAIN_EXTENSION_NAME;
    VkPhysicalDeviceFeatures physical_device_features;
    vkGetPhysicalDeviceFeatures(device_types->physical_device_best_device, &physical_device_features);
    
    VkDeviceCreateInfo device_create_info = create_device_info(queue_families_properties_count, device_queue_create_infos, 1, &required_extension, &physical_device_features);
    
    VK_CHECK(vkCreateDevice(device_types->physical_device_best_device, &device_create_info, vulkan_context->allocator, &logical_device_context->logical_device));
    load_device_level_funcs(&instance_context,logical_device_context);
    
    u32 queue_family_graphics_count = 0;
    u32 queue_family_graphics_list[queue_family_graphics_count];
    for (u32 i = 0; i < queue_families_properties_count; i++) {
        if ((queue_family_properties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0) {
            queue_family_graphics_list[queue_family_graphics_count] = i;
            queue_family_graphics_count++;
        }
    }
    
    u32 max_queue_count = 0;
    get_queue_context->queue_family_best_queue_index = 0;
    for (u32 i = 0; i < queue_family_graphics_count; i++) {
        if (queue_family_properties->queueCount > max_queue_count) {
            get_queue_context->queue_family_best_queue_index = queue_family_graphics_list[i];
        }
    }
    
    printf("best queue family index %d\n", get_queue_context->queue_family_best_queue_index);
    
    vkGetDeviceQueue(logical_device_context->logical_device, get_queue_context->queue_family_best_queue_index, 0, &get_queue_context->graphics_queue);
    get_queue_context->single_queue = 1;
    if (queue_family_properties[get_queue_context->queue_family_best_queue_index].queueCount < 2) {
        vkGetDeviceQueue(logical_device_context->logical_device, get_queue_context->queue_family_best_queue_index, 0, &get_queue_context->present_queue);
        printf("using single queue for drawing\n");
    } else {
        get_queue_context->single_queue = 0;
        vkGetDeviceQueue(logical_device_context->logical_device, get_queue_context->queue_family_best_queue_index, 1, &get_queue_context->present_queue);
        printf("using double queues for rawing");
    };
    
    return VK_SUCCESS;
    
};

*/