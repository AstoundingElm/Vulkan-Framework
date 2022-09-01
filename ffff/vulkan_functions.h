#pragma once

#include "vulkan_types.h"

#include <dlfcn.h>
#include <assert.h>

global_variable void *vulkan_lib;

#define CREATE_INSTANCE_FUNC(name) name = (PFN_##name)vkGetInstanceProcAddr(vulkan_context->instance, #name)

#define CREATE_DEVICE_FUNC(name) name = (PFN_##name)vkGetDeviceProcAddr(vulkan_context->logical_device, #name)

#define func_concat(name) PFN_##name name;

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

    void load_global_level_functions(vulkan_types * load_context) {
    vulkan_lib = dlopen("libvulkan.so", RTLD_NOW);

    vkGetInstanceProcAddr = (PFN_vkGetInstanceProcAddr)dlsym(vulkan_lib, "vkGetInstanceProcAddr");
    vkDestroyInstance = (PFN_vkDestroyInstance)dlsym(vulkan_lib, "vkDestroyInstance");
    vkCreateInstance = (PFN_vkCreateInstance)vkGetInstanceProcAddr(load_context->instance, "vkCreateInstance");
};

PINLINE bool load_instance_level_funcs(vulkan_types * vulkan_context) {
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
    assert(CREATE_INSTANCE_FUNC(vkDestroySurfaceKHR));

    return true;}


PINLINE void load_device_level_funcs(vulkan_types * vulkan_context) {

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
}

