#pragma once
#include "defines.h"
#include "vulkan_types.h"
#include "vulkan_functions.h"

#include <stdio.h>


PINLINE VkDeviceQueueCreateInfo create_device_queue_info(const void * pnext, VkDeviceQueueCreateFlags queue_flags,u32 queue_family_index, u32 queue_count, const float *queue_priorities) {
    VkDeviceQueueCreateInfo queue_create_info = {};
    queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queue_create_info.pNext = pnext;
    queue_create_info.flags = queue_flags;
    queue_create_info.queueFamilyIndex = queue_family_index;
    queue_create_info.queueCount = queue_count;
    queue_create_info.pQueuePriorities = queue_priorities;
    
    return queue_create_info;
};


PINLINE VkDeviceQueueCreateInfo create_queue_info(const void * pnext, u32 queue_family_index, u32 queue_count, const float *queue_priorities) {
    VkDeviceQueueCreateInfo queue_create_info = {};
    queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queue_create_info.pNext = pnext;
    queue_create_info.flags = 0;
    queue_create_info.queueFamilyIndex = queue_family_index;
    queue_create_info.queueCount = queue_count;
    queue_create_info.pQueuePriorities = queue_priorities;
    
    return queue_create_info;
};


PINLINE VkDeviceCreateInfo create_device_info(const void * pnext, VkDeviceCreateFlags device_flags, u32 queue_create_info_count, const VkDeviceQueueCreateInfo *queue_create_infos, u32 enabled_extension_count,
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


PINLINE VkResult create_logical_device( logical_device_types * logical_context, physical_device_types * physical_context, queue_types * queue_context){
    
    float queue_priority = 1.0f;
    
    VkDeviceQueueCreateInfo queue_info = create_device_queue_info(NULL, 0, physical_context->graphics_index, 
                                                                  1, &queue_priority);
    
    const char * extensions[] = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};
    VkDeviceCreateInfo device_info = create_device_info(NULL, 0,1, &queue_info,  ArraySize(extensions), extensions, NULL);
    
    VK_CHECK(vkCreateDevice(physical_context->physical_device_best_device, &device_info, 0, &logical_context->logical_device));
    
    load_device_level_funcs( &logical_device_context);
    
    vkGetDeviceQueue(logical_context->logical_device, physical_context->graphics_index, 0, &queue_context->graphics_queue);
    return VK_SUCCESS;
    
};

PINLINE VkResult pick_physical_device(instance_types * instance_context, physical_device_types * device_context){
    
    device_context->physical_device_count = 0;
    VK_CHECK(vkEnumeratePhysicalDevices( instance_context->instance, &device_context->physical_device_count, NULL));
    // create an array of VkPhysicalDevices of the size returned by the first enumeration.
    VkPhysicalDevice physical_devices[device_context->physical_device_count];
    // enumerate again, and we now have an array of physical devies two be queried.
    VK_CHECK(vkEnumeratePhysicalDevices( instance_context->instance, &device_context->physical_device_count, physical_devices));
    
    VkPhysicalDeviceProperties physical_device_properties[device_context->physical_device_count];
    
    u32 discrete_gpu_list[device_context->physical_device_count];
    u32 discrete_gpu_count = 0;
    u32 integrated_gpu_list[device_context->physical_device_count];
    u32 integrated_gpu_count = 0;
    
    VkPhysicalDeviceMemoryProperties physical_device_memory_properties[device_context->physical_device_count];
    u32 physical_device_memory_count[device_context->physical_device_count];
    VkDeviceSize physical_device_memory_total[device_context->physical_device_count];
    
    for (u32 i = 0; i < device_context->physical_device_count; i++) {
        vkGetPhysicalDeviceProperties(physical_devices[i], &physical_device_properties[i]);
        if (physical_device_properties[i].deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
            discrete_gpu_list[discrete_gpu_count] = i;
            discrete_gpu_count++;
        } else if (physical_device_properties[i].deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU) {
            integrated_gpu_list[integrated_gpu_count] = i;
            integrated_gpu_count++;
        };
        
        vkGetPhysicalDeviceMemoryProperties(physical_devices[i], &physical_device_memory_properties[i]);
        physical_device_memory_count[i] = physical_device_memory_properties[i].memoryHeapCount;
        physical_device_memory_total[i] = 0;
        for (u32 j = 0; j < physical_device_memory_count[i]; j++) {
            physical_device_memory_total[i] += physical_device_memory_properties[i].memoryHeaps[j].size;
        }
    }
    
    VkDeviceSize max_memory_size = 0;
    u32 physical_device_best_index = 0;
    
    if (discrete_gpu_count != 0) {
        for (u32 i = 0; i < discrete_gpu_count; i++) {
            if (physical_device_memory_total[i] > max_memory_size) {
                physical_device_best_index = discrete_gpu_list[i];
                max_memory_size = physical_device_memory_total[i];
            }
        }
        
    } else if (integrated_gpu_count != 0) {
        for (u32 i = 0; i < integrated_gpu_count; i++) {
            if (physical_device_memory_total[i] > max_memory_size) {
                physical_device_best_index = integrated_gpu_list[i];
                max_memory_size = physical_device_memory_total[i];
            }
        }
    }
    
    printf("Best device index: %u\n", physical_device_best_index);
    printf("Device name :%s\n", physical_device_properties[physical_device_best_index].deviceName);
    printf("Device type:");
    if (discrete_gpu_count != 0) {
        printf("Discrete gpu\n");
    } else if (integrated_gpu_count != 0) {
        printf("Integrated gpu\n");
    } else {
        printf("unknown");
    };
    
    const uint64_t mib = 1024 * 1024;
    
    printf("memory total in mibiytes:%lu\n", physical_device_memory_total[physical_device_best_index] / mib);
    device_context->physical_device_best_device = (physical_devices[physical_device_best_index]);
    
    return VK_SUCCESS;
    
};

PINLINE VkResult create_physical_device(physical_device_types * physical_context, instance_types * inst_context, surface_types * surface_context){
    physical_context->graphics_index = -1;
    
    u32 gpu_count = 0;
    
    VkPhysicalDevice gpus[10];
    
    VK_CHECK(vkEnumeratePhysicalDevices(inst_context->instance, &gpu_count, 0));
    VK_CHECK(vkEnumeratePhysicalDevices(inst_context->instance,
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
                
                VK_CHECK(vkGetPhysicalDeviceSurfaceSupportKHR(gpu, j, surface_context->surface, &surface_support));
                if(surface_support){
                    
                    physical_context->graphics_index = j;
                    physical_context->physical_device_best_device = gpu;
                    break;
                    
                }
                
                
            }
            
        }
        
    }
    if(physical_context->graphics_index  < 0){
        
        return VK_ERROR_OUT_OF_HOST_MEMORY;
    }
    
    return VK_SUCCESS;
}