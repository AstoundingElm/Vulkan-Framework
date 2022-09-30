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
#include "logger.h"
#include "entity.h"

int main(){
    //u8 red = 3.14f;
    
    //puts("\033[1;31mThis is bold red text\033[0m\n");
    Game_State game_state = {};
    //platform_console_write_error("hello %f", red);
    //platform_console_write("testing %f", 3.14f);
    init_xwindow();
    if(!init_game(&game_state)){
        
        printf("fail");
        return -1;
        
    };
    
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
    
    create_graphics_pipeline(&pipeline_context, &renderpass_context,&instance_context, &logical_device_context,
                             &descriptor_context);
    
    VK_CHECK(create_texture(&logical_device_context, &physical_device_context,
                            &command_context, &load_texture_data));
    
    VK_CHECK(create_image_view(&logical_device_context));
    VK_CHECK(create_sampler(&logical_device_context, &sampler_context));
    
    transform_storage_buffer = vk_allocate_buffer(
                                                  
                                                  logical_device_context.logical_device,
                                                  physical_device_context.physical_device_best_device,
                                                  sizeof(Transform) * MAX_ENTITIES,
                                                  VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
                                                  VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
    
    
    // Create Global Uniform Buffer Object
    
    globalUBO = vk_allocate_buffer(
                                   logical_device_context.logical_device,
                                   physical_device_context.physical_device_best_device,
                                   sizeof(GlobalData),
                                   VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                                   VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
    
    GlobalData globalData = {
        (int)800,
        (int)600};
    
    vk_copy_to_buffer(&globalUBO, &globalData, sizeof(globalData));
    
    /*
    createBuffer(sizeof(Transform) *MAX_ENTITIES, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, transform_storage_buffer.buffer, transform_storage_buffer.memory, &logical_device_context);
    
    
    
    createBuffer(sizeof(GlobalData), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
                 GlobalUBO.buffer, GlobalUBO.memory, &logical_device_context);
    
    
    GlobalData global_data = {
        (i32)800,
        (i32)600,
        
    };
    
    copyBufferToImage(global_ubo.buffer, global_ubo.image,
                      (i32)global_data.screen_size_x, (i32)global_data.screen_size_y);
    
    
    vk_copy_to_buffer(&GlobalUBO, &global_data, sizeof(GlobalData));
    */
    
    VK_CHECK(create_descriptor_pool(&logical_device_context, &descriptor_context));
    VK_CHECK(create_descriptor_set(&logical_device_context, &descriptor_context,
                                   &sampler_context));
    //while(renderloop_running){
    vulkan_running = true;
    while(vulkan_running){
        
        
        update_game(&game_state);
        
        
        VK_CHECK(render_loop(&logical_device_context, &semaphore_context, &command_context, &swapchain_context, &q_context, &renderpass_context, &framebuffer_context, &pipeline_context, &descriptor_context, &screen_context, &game_state));
        
        xwindow_main_loop();
        
        
    }
    
    
    
};
