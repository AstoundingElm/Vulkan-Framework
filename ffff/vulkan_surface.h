#pragma once

#include "vulkan_functions.h"
#include "vulkan_types.h"
#include "defines.h"
#include "platform.h"
#include "vulkan_queues.h"

struct surface_types{
 VkSurfaceCapabilitiesKHR surface_capabilities;
 u32 surface_format_count;
  VkSurfaceFormatKHR surface_formats[0];
  char extent_suitable;
  VkExtent2D actual_extent;
  char mailbox_mode_supported;
};

surface_types surface_context;

PINLINE VkResult create_surface(vulkan_types * vulkan_context, physical_device_types * device_types, queue_types * queue_context, surface_types * surface_context){

 VkXcbSurfaceCreateInfoKHR surface_create_info = create_xcb_surface(NULL, 0, xcb_context.connection, xcb_context.window);

    VK_CHECK(vkCreateXcbSurfaceKHR(vulkan_context->instance, &surface_create_info, vulkan_context->allocator, &vulkan_context->surface));
    VkBool32 surface_support;

    VK_CHECK(vkGetPhysicalDeviceSurfaceSupportKHR(device_types->physical_device_best_device, queue_context->queue_family_best_queue_index, vulkan_context->surface, &surface_support));

	vkGetPhysicalDeviceSurfaceCapabilitiesKHR
		(device_types->physical_device_best_device, vulkan_context->surface,&surface_context->surface_capabilities);
	printf("fetched caps from surface.\n");
	char extent_suitable=1;
	
     xcb_get_geometry_cookie_t cookie;
    xcb_get_geometry_reply_t *reply;

    cookie = xcb_get_geometry(xcb_context.connection, xcb_context.window);

    if ((reply = xcb_get_geometry_reply(xcb_context.connection, cookie, NULL))) {
        printf("This window is at %d, %d\n", reply->width, reply->height);
    };
    int wind_w,wind_h;
    wind_w = reply->width;
    wind_h = reply->height;
    surface_context-> actual_extent = {reply->width, reply->height};
	if(surface_context->surface_capabilities.currentExtent.width!=wind_w||surface_context->surface_capabilities.currentExtent.height!=wind_h){
		surface_context->extent_suitable=0;
		printf("actual extent size doesn't match framebuffers, resizing...\n");
		surface_context->actual_extent.width=wind_w>surface_context->surface_capabilities.maxImageExtent.width? surface_context->surface_capabilities.maxImageExtent.width:wind_w;
		surface_context->actual_extent.width=wind_w<surface_context->surface_capabilities.minImageExtent.width?surface_context->surface_capabilities.minImageExtent.width:wind_w;
		surface_context->actual_extent.height=wind_h>surface_context->surface_capabilities.maxImageExtent.height?surface_context->surface_capabilities.maxImageExtent.height:wind_h;
		surface_context->actual_extent.height=wind_h<surface_context->surface_capabilities.minImageExtent.height?surface_context->surface_capabilities.minImageExtent.height:wind_h;
	}
    surface_context->surface_format_count = 0;
   
    surface_context->surface_formats[surface_context->surface_format_count];
    vkGetPhysicalDeviceSurfaceFormatsKHR(device_types->physical_device_best_device, vulkan_context->surface, &surface_context->surface_format_count, NULL);
  
    vkGetPhysicalDeviceSurfaceFormatsKHR(device_types->physical_device_best_device, vulkan_context->surface, &surface_context->surface_format_count, surface_context->surface_formats);

    u32 present_mode_count;

    vkGetPhysicalDeviceSurfacePresentModesKHR(device_types->physical_device_best_device, vulkan_context->surface, &present_mode_count, NULL);

    VkPresentModeKHR present_modes[present_mode_count];
    vkGetPhysicalDeviceSurfacePresentModesKHR(device_types->physical_device_best_device, vulkan_context->surface, &present_mode_count, present_modes);


        printf("%d", present_mode_count);
    surface_context->mailbox_mode_supported = 0;
    for (u32 i = 0; i < present_mode_count; i++) {
        	//printf("present mode:%d\n",pres_modes[i]);
		if(present_modes[i]==VK_PRESENT_MODE_MAILBOX_KHR){
			printf("mailbox present mode supported.\n");
			surface_context->mailbox_mode_supported=1;
		}
    };

    return VK_SUCCESS;
}


