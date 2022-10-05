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

static f64 last_time;

struct petes_clock {
    f64 start_time;
    f64 elapsed;
};

static petes_clock actual_clock;

f64 platform_get_absolute_time() {
    struct timespec now;
    clock_gettime(CLOCK_MONOTONIC, &now);
    return now.tv_sec + now.tv_nsec * 0.000000001;
}

void clock_update(petes_clock* clock) {
    if (clock->start_time != 0) {
        clock->elapsed = platform_get_absolute_time() - clock->start_time;
    }
}

void clock_start(petes_clock* clock) {
    clock->start_time = platform_get_absolute_time();
    clock->elapsed = 0;
}

void clock_stop(petes_clock* clock) {
    clock->start_time = 0;
}

int main(){
    
    
    Game_State game_state = {};
    
    init_xwindow(&xcb_context);
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
    
    VK_CHECK(create_instance(&instance_context));
    
    VK_CHECK(create_linux_surface(&instance_context, &surf_context));
    
    
    VK_CHECK(create_physical_device(&physical_device_context, &instance_context,
                                    &surf_context ));
    
    VK_CHECK(create_logical_device( &logical_device_context, &physical_device_context, &q_context));
    
    create_swapchain( &physical_device_context, &surf_context, &logical_device_context, &swapchain_context);
    
    create_renderpass(&surf_context, &logical_device_context, &renderpass_context);
    
    create_framebuffers(&framebuffer_context, &renderpass_context, &swapchain_context, &logical_device_context);
    
    create_command_pool(&physical_device_context, &logical_device_context, &command_context);
    
    create_semaphore_objects(&logical_device_context, &semaphore_context);
    
    create_descriptor_sets(&logical_device_context, &descriptor_context);
    
    create_graphics_pipeline(&pipeline_context, &renderpass_context, &logical_device_context,
                             &descriptor_context);
    
    
    global_image = material_image(&logical_device_context, &load_texture_data, ASSET_SPRITE_DEFAULT);
    
    
    VK_CHECK(create_image_view(&logical_device_context, global_image));
    VK_CHECK(create_sampler(&logical_device_context, &sampler_context));
    
    
    
    transform_storage_buffer = vk_allocate_buffer(
                                                  
                                                  logical_device_context.logical_device,
                                                  physical_device_context.physical_device_best_device,
                                                  sizeof(Transform) * MAX_ENTITIES,
                                                  VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
                                                  VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
    
    
    
    material_storage_buffer = vk_allocate_buffer(
                                                 
                                                 logical_device_context.logical_device,
                                                 physical_device_context.physical_device_best_device,
                                                 sizeof(Material_Data) * MAX_MATERIALS,
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
    
    indexBuffer = vk_allocate_buffer(
                                     logical_device_context.logical_device,
                                     physical_device_context.physical_device_best_device,
                                     sizeof(uint32_t) * 6,
                                     VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
                                     VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    
    
    u32 indices[] = {
        0, 1, 2, 2, 3, 0,
    };
    
    vk_copy_to_buffer(&indexBuffer, &indices, sizeof(u32 ) * 6);
    
    VK_CHECK(create_descriptor_pool(&logical_device_context, &descriptor_context));
    
    struct timespec ticks_per_second, last_tick_count, current_tick_count;
    clock_gettime(CLOCK_MONOTONIC, &ticks_per_second);
    clock_gettime(CLOCK_MONOTONIC, &last_tick_count);
    
    
    
    vulkan_running = true;
    clock_start(&actual_clock);
    clock_update(&actual_clock);
    last_time = actual_clock.elapsed;
    f64 running_time = 0;
    u8 frame_count = 0;
    f64 target_frame_seconds = 1.0f/60;
    
    
    while(vulkan_running){
        xwindow_main_loop(&xcb_context);
        
        clock_update(&actual_clock);
        f64 current_time = actual_clock.elapsed;
        f64 delta = (current_time - last_time);
        f64 frame_start_time = platform_get_absolute_time();
        update_game(&game_state, (f32)delta);
        
        clock_gettime(CLOCK_MONOTONIC, &current_tick_count);
        //u64 elapsed_ticks = current_tick_count - last_tick_count;
        VK_CHECK(render_loop(&logical_device_context, &semaphore_context, &command_context, &swapchain_context, &q_context, &renderpass_context, &framebuffer_context, &pipeline_context, &screen_context, &game_state));
        
        f64 frame_end_time = platform_get_absolute_time();
        f64 frame_elapsed_time = frame_end_time - frame_start_time;
        running_time += frame_elapsed_time;
        frame_count ++;
        
    }
    
    return 0;
    
};
