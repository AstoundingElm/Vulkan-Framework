#pragma once

#include "defines.h"
#include "core/asserts.h"

#include <vulkan/vulkan.h>

// Checks the given expression's return value against VK_SUCCESS.
#define VK_CHECK(expr)               \
    {                                \
        KASSERT(expr == VK_SUCCESS); \
    }

typedef struct vulkan_image {
    /** @brief The handle to the internal image object. */
    VkImage handle;
    /** @brief The memory used by the image. */
    VkDeviceMemory memory;
    /** @brief The view for the image, which is used to access the image. */
    VkImageView view;
    /** @brief The image width. */
    u32 width;
    /** @brief The image height. */
    u32 height;
} vulkan_image;

typedef enum vulkan_render_pass_state {
    READY,
    RECORDING,
    IN_RENDER_PASS,
    RECORDING_ENDED,
    SUBMITTED,
    NOT_ALLOCATED
} vulkan_render_pass_state;

typedef struct vulkan_renderpass {
    VkRenderPass handle;
    f32 x, y, w, h;
    f32 r, g, b, a;

    f32 depth;
    u32 stencil;

    vulkan_render_pass_state state;
} vulkan_renderpass;

typedef struct vulkan_fence {
    VkFence handle;
    b8 is_signaled;
} vulkan_fence;


typedef enum vulkan_command_buffer_state {
    COMMAND_BUFFER_STATE_READY,
    COMMAND_BUFFER_STATE_RECORDING,
    COMMAND_BUFFER_STATE_IN_RENDER_PASS,
    COMMAND_BUFFER_STATE_RECORDING_ENDED,
    COMMAND_BUFFER_STATE_SUBMITTED,
    COMMAND_BUFFER_STATE_NOT_ALLOCATED
} vulkan_command_buffer_state;

typedef struct vulkan_framebuffer {
    VkFramebuffer handle;
    u32 attachment_count;
    VkImageView* attachments;
    vulkan_renderpass* renderpass;
} vulkan_framebuffer;

typedef struct vulkan_command_buffer {
    VkCommandBuffer handle;

    // Command buffer state.
    vulkan_command_buffer_state state;
} vulkan_command_buffer;

typedef struct vulkan_swapchain_support_info {
    VkSurfaceCapabilitiesKHR capabilities;
    u32 format_count;
    VkSurfaceFormatKHR* formats;
    u32 present_mode_count;
    VkPresentModeKHR* present_modes;
} vulkan_swapchain_support_info;

typedef struct vulkan_swapchain {
    /** @brief The swapchain image format. */
    VkSurfaceFormatKHR image_format;
    /**
     * @brief The maximum number of "images in flight" (images simultaneously being rendered to).
     * Typically one less than the total number of images available.
     */
    u8 max_frames_in_flight;

    /** @brief The swapchain internal handle. */
    VkSwapchainKHR handle;
    /** @brief The number of swapchain images. */
    u32 image_count;
    /** @brief An array of pointers to render targets, which contain swapchain images. */
     VkImage* images;
    VkImageView* views;
    
    vulkan_image depth_attachment;

    // framebuffers used for on-screen rendering.
    vulkan_framebuffer* framebuffers;
} vulkan_swapchain;

typedef struct vulkan_device {

    VkFormat depth_format;
    VkPhysicalDevice physical_device;
    VkDevice logical_device;
    vulkan_swapchain_support_info swapchain_support;
   // i32 graphics_queue_index;
   // i32 present_queue_index;
   // i32 transfer_queue_index;
    u32 graphics_index;
    VkPhysicalDeviceProperties properties;
    VkPhysicalDeviceFeatures features;
    VkPhysicalDeviceMemoryProperties memory;
    VkQueue graphics_queue;
VkCommandPool graphics_command_pool;

} vulkan_device;

typedef struct vulkan_context {

    i32 (*find_memory_index)(u32 type_filter, u32 property_flags);
    u32 framebuffer_width;
    u32 framebuffer_height;
    u32 image_index;
    u32 current_frame;
    b8 recreating_swapchain;

    VkInstance instance;
    VkAllocationCallbacks* allocator;
    VkSurfaceKHR surface;
   VkSemaphore* image_available_semaphores;

    // darray
    VkSemaphore* queue_complete_semaphores;

    u32 in_flight_fence_count;
    vulkan_fence* in_flight_fences;
	u64 framebuffer_size_last_generation;
	u64 framebuffer_size_generation;
    // Holds pointers to fences which exist and are owned elsewhere.
    vulkan_fence** images_in_flight;

    vulkan_swapchain swapchain;
    vulkan_command_buffer* graphics_command_buffers;
    vulkan_swapchain_support_info swapchain_support;
#if defined(_DEBUG)
    VkDebugUtilsMessengerEXT debug_messenger;
#endif

    vulkan_device device;
 vulkan_renderpass main_renderpass;
} vulkan_context;