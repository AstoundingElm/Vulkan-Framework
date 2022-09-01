#pragma once
#include "defines.h"
#include "vulkan_types.h"
#include "vulkan_functions.h"

#include "stdio.h"

struct physical_device_types{
u32 physical_device_count;
VkPhysicalDevice physical_device_best_device;

};

global_variable physical_device_types device_context;

PINLINE VkDeviceQueueCreateInfo create_queue_info(u32 queue_family_index, u32 queue_count, const float *queue_priorities) {
    VkDeviceQueueCreateInfo queue_create_info = {VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO};
    // queue_create_info.pNext =
    // queue_create_info.flags =
    queue_create_info.queueFamilyIndex = queue_family_index;
    queue_create_info.queueCount = queue_count;
    queue_create_info.pQueuePriorities = queue_priorities;
    // 0, 0, queue_family_index, queue_count, queue_priorities};
    return queue_create_info;
}

PINLINE VkResult pick_physical_device(vulkan_types * vulkan_context, physical_device_types * device_context){

     device_context->physical_device_count = 0;
    VK_CHECK(vkEnumeratePhysicalDevices(vulkan_context->instance, &device_context->physical_device_count, NULL));
    // create an array of VkPhysicalDevices of the size returned by the first enumeration.
    VkPhysicalDevice physical_devices[device_context->physical_device_count];
    // enumerate again, and we now have an array of physical devies two be queried.
    VK_CHECK(vkEnumeratePhysicalDevices(vulkan_context->instance, &device_context->physical_device_count, physical_devices));

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

}