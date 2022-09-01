#pragma once
#include "defines.h"
#define VK_USE_PLATFORM_XCB_KHR
#define VK_NO_PROTOTYPES
#include <vulkan/vulkan.h>

struct vulkan_types {
    VkInstance instance;
    VkSurfaceKHR surface;
    VkDebugUtilsMessengerEXT debug_messenger;
    VkDevice logical_device;
    VkAllocationCallbacks *allocator;
    VkRenderPass renderpass;
  
};

global_variable vulkan_types vk_context;