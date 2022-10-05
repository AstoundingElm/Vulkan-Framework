#pragma once

#include "vulkan_types.h"

#include <dlfcn.h>
#include <assert.h>

global_variable void *vulkan_lib = NULL;

#define CREATE_INSTANCE_FUNC(name) name = (PFN_##name)vkGetInstanceProcAddr(instance_context->instance, #name)

#define CREATE_DEVICE_FUNC(name) name = (PFN_##name)vkGetDeviceProcAddr(logical_device_context->logical_device, #name)

#define func_concat(name) PFN_##name name;
func_concat(vkCreateDescriptorSetLayout)
func_concat(vkCreateDebugUtilsMessengerEXT)
func_concat(vkEnumeratePhysicalDevices)
func_concat(vkGetPhysicalDeviceQueueFamilyProperties)
func_concat(vkGetInstanceProcAddr)
func_concat(vkCreateInstance)
func_concat(vkCreateXcbSurfaceKHR)
func_concat(vkGetPhysicalDeviceSurfaceSupportKHR)
func_concat(vkCreateDevice)
func_concat(vkGetDeviceProcAddr)
func_concat(vkGetDeviceQueue)
func_concat(vkGetPhysicalDeviceSurfaceFormatsKHR)
func_concat(vkCreateSwapchainKHR)
func_concat(vkGetSwapchainImagesKHR)
func_concat(vkCreateImageView)
func_concat(vkCreateRenderPass)
func_concat(vkAllocateCommandBuffers)
func_concat(vkCreateSemaphore)
func_concat(vkCreateFence)
func_concat(vkCreatePipelineLayout)
func_concat(vkDestroyInstance)
func_concat(vkGetPhysicalDeviceSurfaceCapabilitiesKHR)
func_concat(vkGetPhysicalDeviceMemoryProperties)
func_concat(vkGetPhysicalDeviceProperties)
func_concat(vkGetPhysicalDeviceFeatures)
func_concat(vkGetPhysicalDeviceSurfacePresentModesKHR)
func_concat(vkBeginCommandBuffer)
func_concat(vkCreateShaderModule)
func_concat(vkCreateGraphicsPipelines)
func_concat(vkDestroyShaderModule)
func_concat(vkCreateFramebuffer)
func_concat(vkCreateCommandPool)
func_concat(vkCmdBeginRenderPass)
func_concat(vkCmdBindPipeline)
func_concat(vkCmdEndRenderPass)
func_concat(vkEndCommandBuffer)
func_concat(vkWaitForFences)
func_concat(vkAcquireNextImageKHR)
func_concat(vkResetFences)
func_concat(vkQueuePresentKHR)
func_concat(vkQueueSubmit)
func_concat(vkCmdDraw)
func_concat(vkDestroyDevice)
func_concat(vkDestroyFramebuffer)
func_concat(vkDestroyCommandPool)
func_concat(vkDestroyFence)
func_concat(vkDeviceWaitIdle)
func_concat(vkFreeCommandBuffers)
func_concat(vkDestroySemaphore)
func_concat(vkDestroyPipeline)
func_concat(vkDestroyPipelineLayout)
func_concat(vkDestroyRenderPass)
func_concat(vkDestroyImageView)
func_concat(vkDestroySwapchainKHR)
func_concat(vkDestroySurfaceKHR)
func_concat(vkDestroyDebugUtilsMessengerEXT)
func_concat(vkCreateBuffer)
func_concat(vkGetBufferMemoryRequirements)
func_concat(vkCreateImage)
func_concat(vkGetImageMemoryRequirements)
func_concat(vkAllocateMemory)
func_concat(vkBindImageMemory)
func_concat(vkCmdPipelineBarrier)
func_concat(vkCmdCopyBufferToImage)
func_concat(vkMapMemory)
func_concat(vkBindBufferMemory)
func_concat(vkUnmapMemory)
func_concat(vkDestroyBuffer)
func_concat(vkFreeMemory)
func_concat(vkQueueWaitIdle)
func_concat(vkCreateSampler)
func_concat(vkAllocateDescriptorSets)
func_concat(vkCreateDescriptorPool)
func_concat(vkUpdateDescriptorSets)
func_concat(vkResetCommandBuffer)
func_concat(vkCmdSetViewport)
func_concat(vkCmdSetScissor)
func_concat(vkCmdBindDescriptorSets)

func_concat(vkCmdBindIndexBuffer)
func_concat(vkCmdDrawIndexed)
func_concat(vkCmdPushConstants)

void load_global_level_functions(instance_types * instance_context) {
    vulkan_lib = {};
    vulkan_lib = dlopen("libvulkan.so", RTLD_NOW);
    
    vkGetInstanceProcAddr = (PFN_vkGetInstanceProcAddr)dlsym(vulkan_lib, "vkGetInstanceProcAddr");
    vkDestroyInstance = (PFN_vkDestroyInstance)dlsym(vulkan_lib, "vkDestroyInstance");
    vkCreateInstance = (PFN_vkCreateInstance)vkGetInstanceProcAddr(instance_context->instance, "vkCreateInstance");
};

PINLINE bool load_instance_level_funcs(instance_types * instance_context) {
    CREATE_INSTANCE_FUNC(vkCreateDebugUtilsMessengerEXT);
    CREATE_INSTANCE_FUNC(vkCreateXcbSurfaceKHR);
    CREATE_INSTANCE_FUNC(vkEnumeratePhysicalDevices);
    CREATE_INSTANCE_FUNC(vkGetPhysicalDeviceQueueFamilyProperties);
    CREATE_INSTANCE_FUNC(vkGetPhysicalDeviceSurfaceSupportKHR);
    CREATE_INSTANCE_FUNC(vkCreateDevice);
    CREATE_INSTANCE_FUNC(vkGetDeviceProcAddr);
    CREATE_INSTANCE_FUNC(vkGetPhysicalDeviceSurfaceCapabilitiesKHR);
    CREATE_INSTANCE_FUNC(vkGetPhysicalDeviceMemoryProperties);
    CREATE_INSTANCE_FUNC(vkGetPhysicalDeviceProperties);
    CREATE_INSTANCE_FUNC(vkGetPhysicalDeviceFeatures);
    CREATE_INSTANCE_FUNC(vkGetPhysicalDeviceSurfaceFormatsKHR);
    CREATE_INSTANCE_FUNC(vkGetPhysicalDeviceSurfacePresentModesKHR);
    CREATE_INSTANCE_FUNC(vkDestroyDebugUtilsMessengerEXT);
    
    CREATE_INSTANCE_FUNC(vkDestroySurfaceKHR);
    
    return true;}


PINLINE void load_device_level_funcs( logical_device_types * logical_device_context) {
    
    CREATE_DEVICE_FUNC(vkGetDeviceQueue);
    CREATE_DEVICE_FUNC(vkCreateSwapchainKHR);
    CREATE_DEVICE_FUNC(vkGetSwapchainImagesKHR);
    CREATE_DEVICE_FUNC(vkCreateImageView);
    CREATE_DEVICE_FUNC(vkCreateRenderPass);
    CREATE_DEVICE_FUNC(vkAllocateCommandBuffers);
    CREATE_DEVICE_FUNC(vkCreateSemaphore);
    CREATE_DEVICE_FUNC(vkCreateFence);
    CREATE_DEVICE_FUNC(vkCreatePipelineLayout);
    CREATE_DEVICE_FUNC(vkCreateShaderModule);
    CREATE_DEVICE_FUNC(vkCreateGraphicsPipelines);
    CREATE_DEVICE_FUNC(vkDestroyShaderModule);
    CREATE_DEVICE_FUNC(vkCreateFramebuffer);
    CREATE_DEVICE_FUNC(vkCreateCommandPool);
    CREATE_DEVICE_FUNC(vkBeginCommandBuffer);
    CREATE_DEVICE_FUNC(vkCmdBeginRenderPass);
    CREATE_DEVICE_FUNC(vkCmdBindPipeline);
    CREATE_DEVICE_FUNC(vkCmdEndRenderPass);
    CREATE_DEVICE_FUNC(vkEndCommandBuffer);
    CREATE_DEVICE_FUNC(vkWaitForFences);
    CREATE_DEVICE_FUNC(vkAcquireNextImageKHR);
    CREATE_DEVICE_FUNC(vkResetFences);
    CREATE_DEVICE_FUNC(vkQueueSubmit);
    CREATE_DEVICE_FUNC(vkQueuePresentKHR);
    CREATE_DEVICE_FUNC(vkCmdDraw);
    CREATE_DEVICE_FUNC(vkDestroyDevice);
    CREATE_DEVICE_FUNC(vkDestroyFence);
    CREATE_DEVICE_FUNC(vkDeviceWaitIdle);
    CREATE_DEVICE_FUNC(vkDestroyFramebuffer);
    CREATE_DEVICE_FUNC(vkFreeCommandBuffers);
    CREATE_DEVICE_FUNC(vkDestroySemaphore);
    CREATE_DEVICE_FUNC(vkDestroyPipeline);
    CREATE_DEVICE_FUNC(vkDestroyPipelineLayout);
    CREATE_DEVICE_FUNC(vkDestroyRenderPass);
    CREATE_DEVICE_FUNC(vkDestroyImageView);
    CREATE_DEVICE_FUNC(vkDestroySwapchainKHR);
    CREATE_DEVICE_FUNC(vkDestroyCommandPool);
    CREATE_DEVICE_FUNC(vkCreateDescriptorSetLayout);
    CREATE_DEVICE_FUNC(vkCreateBuffer);
    CREATE_DEVICE_FUNC(vkGetBufferMemoryRequirements);
    CREATE_DEVICE_FUNC(vkCreateImage);
    CREATE_DEVICE_FUNC(vkGetImageMemoryRequirements);
    CREATE_DEVICE_FUNC(vkGetBufferMemoryRequirements);
    CREATE_DEVICE_FUNC(vkBindImageMemory);
    CREATE_DEVICE_FUNC(vkCmdPipelineBarrier);
    CREATE_DEVICE_FUNC(vkCmdCopyBufferToImage);
    CREATE_DEVICE_FUNC(vkAllocateMemory);
    CREATE_DEVICE_FUNC(vkMapMemory);
    CREATE_DEVICE_FUNC(vkBindBufferMemory);
    CREATE_DEVICE_FUNC(vkUnmapMemory);
    CREATE_DEVICE_FUNC(vkDestroyBuffer);
    CREATE_DEVICE_FUNC(vkFreeMemory);
    CREATE_DEVICE_FUNC(vkQueueWaitIdle);
    CREATE_DEVICE_FUNC(vkCreateSampler);
    CREATE_DEVICE_FUNC(vkAllocateDescriptorSets);
    CREATE_DEVICE_FUNC(vkCreateDescriptorPool);
    CREATE_DEVICE_FUNC(vkUpdateDescriptorSets);
    CREATE_DEVICE_FUNC(vkResetCommandBuffer);
    CREATE_DEVICE_FUNC(vkCmdSetViewport);
    CREATE_DEVICE_FUNC(vkCmdSetScissor);
    CREATE_DEVICE_FUNC(vkCmdBindDescriptorSets);
    CREATE_DEVICE_FUNC(vkCmdDrawIndexed);
    CREATE_DEVICE_FUNC(vkCmdBindIndexBuffer);
    CREATE_DEVICE_FUNC(vkCmdPushConstants);
}


