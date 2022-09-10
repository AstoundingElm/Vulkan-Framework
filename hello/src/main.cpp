#include <stdio.h>
#include "render_loop.h"
#include "defines.h"
#include "platform.h"
#include "vulkan_functions.h"
#include "vulkan_device.h"
#include "shader_utils.h"
#include "vulkan_instance.h"
#include "vulkan_surface.h"
#include "vulkan_swapchain.h"
#include "vulkan_renderpass.h"
#include "vulkan_framebuffers.h"
#include "command_buffers.h"
#include "vulkan_semaphore.h"
#include "descriptor_sets.h"
#include "vulkan_pipeline.h"
#include "textures.h"
#include "render_loop.h"
#include <assert.h>



int main(){
    
    init_xwindow();
    
    
    xcb_get_geometry_cookie_t cookie;
    xcb_get_geometry_reply_t *reply;
    
    cookie = xcb_get_geometry(xcb_context.connection, xcb_context.window);
    
    if ((reply = xcb_get_geometry_reply(xcb_context.connection, cookie, NULL))) {
        printf("This window is at %d, %d\n", reply->width, reply->height);
    };
    
    screen_context.screen_size.width = reply->width;
    screen_context.screen_size.height  = reply->height;
    
    //VkExtent2D extent = {reply->width, reply->height};
    
    VK_CHECK(create_instance(&instance_context));
    
    VK_CHECK(create_linux_surface(&instance_context, &surf_context));
    
    VK_CHECK(create_physical_device(&physical_device_context, &instance_context,
                                    &surf_context ));
    
    VK_CHECK(create_logical_device(&instance_context, &logical_device_context, &physical_device_context, &q_context));
    
    create_swapchain(&instance_context, &physical_device_context, &surf_context, &logical_device_context, &swapchain_context);
    
    create_renderpass(&surf_context, &logical_device_context, &renderpass_context);
    
    create_framebuffers(&framebuffer_context, &renderpass_context, &swapchain_context, &logical_device_context);
    
    create_command_pool(&physical_device_context, &logical_device_context, &command_context);
    
    create_semaphore_objects(&logical_device_context, &semaphore_context);
    
    create_descriptor_sets(&logical_device_context, &descriptor_context);
    
    VK_CHECK(create_graphics_pipeline(&pipeline_context, &renderpass_context,&instance_context, &logical_device_context,
                                      &descriptor_context));
    
    VK_CHECK(create_texture(&logical_device_context, &physical_device_context,
                            &command_context, &load_texture_data));
    
    VK_CHECK(create_image_view(&logical_device_context));
    VK_CHECK(create_sampler(&logical_device_context, &sampler_context));
    VK_CHECK(create_descriptor_pool(&logical_device_context, &descriptor_context));
    VK_CHECK(create_descriptor_set(&logical_device_context, &descriptor_context,
                                   &sampler_context));
    //while(renderloop_running){
    vulkan_running = true;
    while(vulkan_running){
        
        VK_CHECK(render_loop(&logical_device_context, &semaphore_context, &command_context, &swapchain_context, &q_context, &renderpass_context, &framebuffer_context, &pipeline_context, &descriptor_context, &screen_context));
        
        xwindow_main_loop();
        
        
    }
    
    
    
};
