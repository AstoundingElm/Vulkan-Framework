#pragma once
#include "defines.h"
#define VK_USE_PLATFORM_XCB_KHR
#define VK_NO_PROTOTYPES

#define STB_IMAGE_IMPLEMENTATION
#include "../stb_image.h"
#include <vulkan/vulkan.h>

struct instance_types{
    
    VkInstance instance;
    VkDebugUtilsMessengerEXT debug_messenger;
    VkAllocationCallbacks * allocator;
    
};

struct physical_device_types{
    
    u32 physical_device_count;
    VkPhysicalDevice physical_device_best_device;
    u32 graphics_index;
    
    
};

struct logical_device_types{
    
    
    VkDevice logical_device;
    
};

struct DescriptorInfo
{
    union
    {
        VkDescriptorBufferInfo bufferInfo;
        VkDescriptorImageInfo imageInfo;
    };
    
    DescriptorInfo(VkSampler sampler, VkImageView imageView)
    {
        imageInfo.sampler = sampler;
        imageInfo.imageView = imageView;
        imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    }
    
    DescriptorInfo(VkBuffer buffer)
    {
        bufferInfo.buffer = buffer;
        bufferInfo.offset = 0;
        bufferInfo.range = VK_WHOLE_SIZE;
    }
    
    DescriptorInfo(VkBuffer buffer, uint32_t offset, uint32_t range)
    {
        bufferInfo.buffer = buffer;
        bufferInfo.offset = offset;
        bufferInfo.range = range;
    }
};

VkWriteDescriptorSet write_set(
                               VkDescriptorSet set,
                               VkDescriptorType type,
                               DescriptorInfo *descInfo,
                               uint32_t bindingNumber,
                               uint32_t count)
{
    VkWriteDescriptorSet write = {};
    write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    write.dstSet = set;
    write.pImageInfo = &descInfo->imageInfo;
    write.pBufferInfo = &descInfo->bufferInfo;
    write.dstBinding = bindingNumber;
    write.descriptorCount = count;
    write.descriptorType = type;
    
    return write;
};

struct surface_types{
    
    VkSurfaceCapabilitiesKHR surface_capabilities;
    u32 surface_format_count;
    VkSurfaceFormatKHR  surface_formats;
    char extent_suitable;
    VkExtent2D actual_extent;
    char mailbox_mode_supported;
    
    VkSurfaceKHR surface;
    
};


struct queue_types{
    u32 queue_family_best_queue_index;
    char single_queue;
    VkQueue graphics_queue;
    VkQueue  present_queue;
    
};

bool renderloop_running = true;

struct swapchain_types{
    
    VkSwapchainKHR swapchain;
    VkImage swapchain_images[5];
    VkImageView swapchain_image_views[5];
    u32 swapchain_image_count;
};

struct renderpass_types{
    
    VkRenderPass renderpass;
    
};

struct framebuffer_types{
    
    VkFramebuffer framebuffers[5];
    
};

struct command_pool_types{
    
    VkCommandPool command_pool;
    VkCommandBuffer command_buffer;
    
};

struct semaphore_types{
    
    VkSemaphore acquire_semaphore;
    VkSemaphore submit_semaphore;
    VkFence image_available_fences;
};

struct descriptor_set_types{
    
    VkDescriptorSetLayout set_layout;
    VkDescriptorPool descriptor_pool;
    VkDescriptorSet descriptor_set;
};

struct pipeline_types{
    VkPipelineLayout pipeline_layout;
    VkPipeline pipeline;
    
};

struct sampler_types{
    
    VkSampler sampler;
    
};

struct screen_types{
    
    VkExtent2D screen_size;
};

global_variable screen_types screen_context{800, 600};

global_variable sampler_types sampler_context;

global_variable pipeline_types pipeline_context;

global_variable descriptor_set_types descriptor_context;

global_variable semaphore_types semaphore_context;

global_variable command_pool_types command_context;

global_variable framebuffer_types framebuffer_context;

global_variable renderpass_types renderpass_context;

global_variable swapchain_types swapchain_context;

global_variable queue_types q_context;


global_variable logical_device_types logical_device_context;

global_variable physical_device_types physical_device_context;

global_variable instance_types instance_context;

global_variable surface_types surf_context;
