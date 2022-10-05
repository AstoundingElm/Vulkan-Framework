#pragma once

#include "vulkan_functions.h"
#include "vulkan_types.h"
#include "defines.h"
#include "platform.h"

PINLINE VkResult create_linux_surface(instance_types * instance_context, surface_types * surface_context){
    
    
    VkXcbSurfaceCreateInfoKHR surface_create_info = create_xcb_surface(NULL, 0, &xcb_context);
    
    VK_CHECK(vkCreateXcbSurfaceKHR(instance_context->instance, &surface_create_info, 0, &surface_context->surface));
    return VK_SUCCESS;
    
}

