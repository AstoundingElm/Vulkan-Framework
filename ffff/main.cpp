#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <dlfcn.h>

#define VK_USE_PLATFORM_XCB_KHR
#define VK_NO_PROTOTYPES

#include <vulkan/vulkan.h>
#include <vulkan/vulkan_xcb.h>
#include <xcb/xcb.h>
#include <X11/keysym.h>
#include <X11/XKBlib.h>
#include <X11/Xlib.h>
#include <X11/Xlib-xcb.h>
#include <sys/time.h>
#include <xkbcommon/xkbcommon.h>
#include <xkbcommon/xkbcommon-x11.h>
#include <string.h>
#include <stdarg.h>
#include <limits.h>
#include <time.h>
#include <stdint.h>

#define global_variable static
#define internal static
#define local_persist static

global_variable void *vulkan_lib;

struct vulkan_types {
    VkInstance instance;
    VkSurfaceKHR surface;
    VkDebugUtilsMessengerEXT debug_messenger;
    uint32_t graphics_index;
    VkPhysicalDevice struct_gpu;
    VkDevice logical_device;
    VkQueue graphics_queue;
    VkSurfaceFormatKHR context_surface_format;
    VkSwapchainKHR swapchain;
    uint32_t swap_img_count;
    VkImage sc_images[5];
    VkImageView swap_image_views[5];
    VkAllocationCallbacks *allocator;
    VkRenderPass renderpass;
    VkFramebuffer framebuffers[5];
    VkExtent2D screen_size;
    VkCommandPool command_pool;
    VkCommandBuffer command_buffer;
    VkFence image_available_fences;
    VkSemaphore submit_semaphore;
    VkSemaphore aquire_semaphore;
    VkPipelineLayout pipeline_layout;
    VkImageView test_image_view;
    VkImage test_image;
    uint32_t physical_device_number;
};

static vulkan_types vk_context;

#define ArraySize(arr) sizeof((arr)) / sizeof((arr[0]))

#define VK_CHECK(result)                    \
    if (result != VK_SUCCESS) {             \
        printf("Vulkan Error: %d", result); \
        __builtin_trap();                   \
    }

struct xcb_types {
    Display *display;
    xcb_connection_t *connection;
    xcb_window_t window;
    xcb_screen_t *screen;
    xcb_atom_t wm_protocols;
    xcb_atom_t wm_delete_win;
    uint32_t WINDOW_WIDTH = 800;
    uint32_t WINDOW_HEIGHT = 600;
    bool running = false;
};

static xcb_types xcb_context;

VKAPI_ATTR VkBool32 VKAPI_CALL vk_debug_callback(
    VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
    VkDebugUtilsMessageTypeFlagsEXT message_types,
    const VkDebugUtilsMessengerCallbackDataEXT *callback_data,
    void *user_data) {
    switch (message_severity) {
        default:
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
            printf("%s", callback_data->pMessage);
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
            printf("%s", callback_data->pMessage);
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
            printf("%s", callback_data->pMessage);
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
            printf("%s", callback_data->pMessage);
            break;
    }
    return VK_FALSE;
};

void init_xwindow() {
    int screenp = 0;
    xcb_context.connection = xcb_connect(NULL, &screenp);
    if (xcb_connection_has_error(xcb_context.connection))
        printf("Failed to connect to X server using XCB.");

    xcb_screen_iterator_t iter =
        xcb_setup_roots_iterator(xcb_get_setup(xcb_context.connection));

    for (int s = screenp; s > 0; s--)
        xcb_screen_next(&iter);
    xcb_context.screen = iter.data;
    xcb_context.window = xcb_generate_id(xcb_context.connection);
    uint32_t eventMask = XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK;
    uint32_t valueList[] = {xcb_context.screen->black_pixel, 0, XCB_EVENT_MASK_BUTTON_PRESS | XCB_EVENT_MASK_BUTTON_RELEASE | XCB_EVENT_MASK_KEY_PRESS | XCB_EVENT_MASK_KEY_RELEASE | XCB_EVENT_MASK_EXPOSURE | XCB_EVENT_MASK_POINTER_MOTION | XCB_EVENT_MASK_STRUCTURE_NOTIFY};
    xcb_create_window(xcb_context.connection, XCB_COPY_FROM_PARENT, xcb_context.window, xcb_context.screen->root, 0, 0,
                      xcb_context.WINDOW_HEIGHT, xcb_context.WINDOW_WIDTH, 0,
                      XCB_WINDOW_CLASS_INPUT_OUTPUT, xcb_context.screen->root_visual, eventMask, valueList);
    const char *applicationName = "Hlll";
    xcb_change_property(
        xcb_context.connection,
        XCB_PROP_MODE_REPLACE,
        xcb_context.window,
        XCB_ATOM_WM_NAME,
        XCB_ATOM_STRING,
        8,
        strlen(applicationName),
        applicationName);

    xcb_map_window(xcb_context.connection, xcb_context.window);
    xcb_flush(xcb_context.connection);
    xcb_intern_atom_cookie_t wmDeleteCookie = xcb_intern_atom(
        xcb_context.connection, 0, strlen("WM_DELETE_WINDOW"), "WM_DELETE_WINDOW");

    xcb_intern_atom_cookie_t wmProtocolsCookie =
        xcb_intern_atom(xcb_context.connection, 0, strlen("WM_PROTOCOLS"), "WM_PROTOCOLS");
    xcb_intern_atom_reply_t *wmDeleteReply =
        xcb_intern_atom_reply(xcb_context.connection, wmDeleteCookie, NULL);
    xcb_intern_atom_reply_t *wmProtocolsReply =
        xcb_intern_atom_reply(xcb_context.connection, wmProtocolsCookie, NULL);
    xcb_context.wm_delete_win = wmDeleteReply->atom;

    xcb_context.wm_protocols = wmProtocolsReply->atom;

    xcb_change_property(xcb_context.connection, XCB_PROP_MODE_REPLACE, xcb_context.window,
                        wmProtocolsReply->atom, 4, 32, 1, &wmDeleteReply->atom);
    xcb_map_window(xcb_context.connection, xcb_context.window);
    xcb_flush(xcb_context.connection);
};

void xwindow_main_loop() {
    xcb_context.running = true;
    xcb_generic_event_t *event;
    xcb_client_message_event_t *cm;

    while (xcb_context.running) {
        event = xcb_wait_for_event(xcb_context.connection);

        switch (event->response_type & ~0x80) {
            case XCB_CLIENT_MESSAGE: {
                cm = (xcb_client_message_event_t *)event;

                if (cm->data.data32[0] == xcb_context.wm_delete_win)

                    xcb_context.running = false;

                break;
            }
        }

        free(event);
    }

    xcb_destroy_window(xcb_context.connection, xcb_context.window);
};

VkCommandBufferBeginInfo cmd_begin_info() {
    VkCommandBufferBeginInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    return info;
};

VkFenceCreateInfo fence_create_info(VkFenceCreateFlags flags = 0) {
    VkFenceCreateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    info.flags = flags;
    return info;
}

VkCommandBufferAllocateInfo cmd_alloc_info(VkCommandPool pool, VkCommandBufferLevel command_buffer_level, uint32_t command_buffer_count) {
    VkCommandBufferAllocateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    info.commandPool = pool;
    info.level = command_buffer_level;
  info.commandBufferCount = command_buffer_count;

    return info;
}

VkApplicationInfo create_app_info(const char *app_name, uint32_t app_version, const char *engine_name, uint32_t engine_version, uint32_t api_version) {
    VkApplicationInfo app_info = {VK_STRUCTURE_TYPE_APPLICATION_INFO};
    // app_info.pNext =
    app_info.pApplicationName = app_name;
    app_info.applicationVersion = app_version;
    app_info.pEngineName = engine_name;
    app_info.engineVersion = engine_version;
    app_info.apiVersion = api_version;
    // 0, app_name, app_version, engine_name, engine_version, api_version};
    return app_info;
};

VkInstanceCreateInfo create_instance_info(VkApplicationInfo *application_info, uint32_t enabled_layer_count, const char *const *enabled_layer_names,
                                          uint32_t enabled_extension_count,
                                          const char *const *enabled_extension_names) {
    VkInstanceCreateInfo instance_create_info = {VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO};
    // instance_create_info.pNext =
    // instance_create_info.flags =
    instance_create_info.pApplicationInfo = application_info;
    instance_create_info.enabledLayerCount = enabled_layer_count;
    instance_create_info.ppEnabledLayerNames = enabled_layer_names;
    instance_create_info.enabledExtensionCount = enabled_extension_count;
    instance_create_info.ppEnabledExtensionNames = enabled_extension_names;

    return instance_create_info;
};

VkXcbSurfaceCreateInfoKHR create_xcb_surface(xcb_connection_t *connection, xcb_window_t window) {
    VkXcbSurfaceCreateInfoKHR surface_create_info = {VK_STRUCTURE_TYPE_XCB_SURFACE_CREATE_INFO_KHR};
    // surface_create_info.pNext =
    // surface_create_info.flags =
    surface_create_info.connection = connection;
    surface_create_info.window = window;

    return surface_create_info;
};

VkDebugUtilsMessengerCreateInfoEXT create_debug_info(VkDebugUtilsMessageSeverityFlagsEXT message_severity, VkDebugUtilsMessageTypeFlagsEXT message_type,
                                                     PFN_vkDebugUtilsMessengerCallbackEXT pfn_user_callback) {
    VkDebugUtilsMessengerCreateInfoEXT debug_create_info = {VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT};
    // debug_create_info.pNext =
    // debug_create_info.flags =
    debug_create_info.messageSeverity = message_severity;
    debug_create_info.messageType = message_type;
    debug_create_info.pfnUserCallback = pfn_user_callback;
    // debug_create_info.pUserData =

    return debug_create_info;
}

VkDeviceQueueCreateInfo create_queue_info(uint32_t queue_family_index, uint32_t queue_count, const float *queue_priorities) {
    VkDeviceQueueCreateInfo queue_create_info = {VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO};
    // queue_create_info.pNext =
    // queue_create_info.flags =
    queue_create_info.queueFamilyIndex = queue_family_index;
    queue_create_info.queueCount = queue_count;
    queue_create_info.pQueuePriorities = queue_priorities;
    // 0, 0, queue_family_index, queue_count, queue_priorities};
    return queue_create_info;
}

VkDeviceCreateInfo create_device_info(uint32_t queue_create_info_count, const VkDeviceQueueCreateInfo *queue_create_infos, uint32_t enabled_extension_count,
                                      const char *const *enabled_extensions_names, const VkPhysicalDeviceFeatures *enabled_features) {
    VkDeviceCreateInfo device_create_info = {VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO};
    // device_create_info.pNext =
    // device_create_info.flags =
    device_create_info.queueCreateInfoCount = queue_create_info_count;
    device_create_info.pQueueCreateInfos = queue_create_infos;
    // device_create_info.enabledLayerCount =
    // device_create_info.ppEnabledLayerNames =
    device_create_info.enabledExtensionCount = enabled_extension_count;
    device_create_info.ppEnabledExtensionNames = enabled_extensions_names;
    device_create_info.pEnabledFeatures = enabled_features;

    return device_create_info;
};

VkSwapchainCreateInfoKHR create_swap_info(VkSurfaceKHR surface, uint32_t min_image_count, VkFormat image_format,
                                          VkColorSpaceKHR image_color_space, VkExtent2D image_extent,
                                          uint32_t image_array_layers, VkImageUsageFlags image_usage, VkSharingMode image_sharing_mode,
                                          uint32_t queue_family_index_count, const uint32_t *queue_family_indices,
                                          VkSurfaceTransformFlagBitsKHR pretransform, VkCompositeAlphaFlagBitsKHR composite_alpha, VkPresentModeKHR present_mode,
                                          VkBool32 b_clipped, VkSwapchainKHR old_swapchain) {
    VkSwapchainCreateInfoKHR swap_info_create = {};

    swap_info_create.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swap_info_create.pNext = NULL;
    swap_info_create.flags = 0;
    swap_info_create.surface = surface;
    swap_info_create.minImageCount = min_image_count;
    swap_info_create.imageFormat = image_format;
    swap_info_create.imageColorSpace = image_color_space;
    swap_info_create.imageExtent = image_extent;
    swap_info_create.imageArrayLayers = image_array_layers;
    swap_info_create.imageUsage = image_usage;
    swap_info_create.imageSharingMode = image_sharing_mode;
    swap_info_create.queueFamilyIndexCount = queue_family_index_count;
    swap_info_create.pQueueFamilyIndices = queue_family_indices;
    swap_info_create.preTransform = pretransform;
    swap_info_create.compositeAlpha = composite_alpha;
    swap_info_create.presentMode = present_mode;
    swap_info_create.clipped = b_clipped;
    swap_info_create.oldSwapchain = old_swapchain;

    return swap_info_create;
};

VkImageViewCreateInfo create_image_view_info(VkImage image, VkImageViewType viewtype, VkFormat format) {
    VkImageViewCreateInfo image_view_create_info = {};

    image_view_create_info.sType = {VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO};
    // image_view_create_info.pNext =
    image_view_create_info.image = image;
    image_view_create_info.viewType = viewtype;
    image_view_create_info.format = format;

    image_view_create_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    image_view_create_info.subresourceRange.levelCount = 1;
    image_view_create_info.subresourceRange.layerCount = 1;

    return image_view_create_info;
};

VkAttachmentDescription create_attachment_description(VkAttachmentDescriptionFlags attachment_flags, VkFormat attachment_format,
                                                      VkSampleCountFlagBits sample_count_bits, VkAttachmentLoadOp load_op, VkAttachmentStoreOp store_op, VkAttachmentLoadOp stencil_load_op,
                                                      VkAttachmentStoreOp stencil_store_op, VkImageLayout initial_layout, VkImageLayout final_layout) {
    VkAttachmentDescription colour_attachment;
    colour_attachment.flags = attachment_flags;
    colour_attachment.format = attachment_format;
    colour_attachment.samples = sample_count_bits;
    colour_attachment.loadOp = load_op;
    colour_attachment.storeOp = store_op;
    colour_attachment.stencilLoadOp = stencil_load_op;
    colour_attachment.stencilStoreOp = stencil_store_op;
    colour_attachment.initialLayout = initial_layout;
    colour_attachment.finalLayout = final_layout;

    return colour_attachment;
};

VkAttachmentReference create_attachment_reference(uint32_t attach, VkImageLayout attachment_reference_layout) {
    VkAttachmentReference attachment_reference = {};
    attachment_reference.attachment = attach;
    attachment_reference.layout = attachment_reference_layout;

    return attachment_reference;
};

VkSubpassDescription create_subpass_desc(VkSubpassDescriptionFlags subpass_description_flags, VkPipelineBindPoint pipeline_bind_point,
                                         uint32_t input_attachment_count, const VkAttachmentReference *input_attachments,
                                         uint32_t colour_attachment_count, const VkAttachmentReference *colour_attachments, const VkAttachmentReference *resolve_attachments,
                                         const VkAttachmentReference *depth_stencil_attachment_reference, uint32_t preserve_attachment_count, const uint32_t *preserve_attachments) {
    VkSubpassDescription subpass_desc = {};
    subpass_desc.flags = subpass_description_flags;
    subpass_desc.pipelineBindPoint = pipeline_bind_point;
    subpass_desc.inputAttachmentCount = input_attachment_count;
    subpass_desc.pInputAttachments = input_attachments;
    subpass_desc.colorAttachmentCount = colour_attachment_count;
    subpass_desc.pColorAttachments = colour_attachments;
    subpass_desc.pResolveAttachments = resolve_attachments;
    subpass_desc.pDepthStencilAttachment = depth_stencil_attachment_reference;
    subpass_desc.preserveAttachmentCount = preserve_attachment_count;
    subpass_desc.pPreserveAttachments = preserve_attachments;
    return subpass_desc;
}

VkRenderPassCreateInfo create_renderpass_info(const void *pointer_next, VkRenderPassCreateFlags renderpass_flags,
                                              uint32_t attachment_count, const VkAttachmentDescription *p_attachments, uint32_t subpass_count,
                                              const VkSubpassDescription *sub_passes, uint32_t dependency_count, const VkSubpassDependency *subpass_dependencies) {
    VkRenderPassCreateInfo renderpass_create_info = {VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO};
    renderpass_create_info.pNext = pointer_next;
    renderpass_create_info.flags = renderpass_flags;
    renderpass_create_info.attachmentCount = attachment_count;
    renderpass_create_info.pAttachments = p_attachments;
    renderpass_create_info.subpassCount = subpass_count;
    renderpass_create_info.pSubpasses = sub_passes;
    renderpass_create_info.dependencyCount = dependency_count;
    renderpass_create_info.pDependencies = subpass_dependencies;

    return renderpass_create_info;
};

VkFramebufferCreateInfo create_framebuffer_info(VkRenderPass render_pass, uint32_t attachment_count, const VkImageView * pattachments, 
uint32_t ren_pass_width, uint32_t ren_pass_height,
                                                uint32_t ren_pass_layers) {
    VkFramebufferCreateInfo framebuffer_info = {VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO};
    framebuffer_info.pNext = NULL;
    framebuffer_info.flags = 0;
    framebuffer_info.renderPass = render_pass;
    framebuffer_info.attachmentCount = attachment_count;
     framebuffer_info.pAttachments = pattachments;
    framebuffer_info.width = ren_pass_width;
    framebuffer_info.height = ren_pass_height;
    framebuffer_info.layers = ren_pass_layers;

    return framebuffer_info;
};

VkPipelineVertexInputStateCreateInfo create_vertex_input_state(uint32_t vertex_binding_description_count, const VkVertexInputBindingDescription *
pvertex_binding_descriptions, uint32_t vertex_attribute_description_count, const VkVertexInputAttributeDescription * pvertex_attribute_descriptions ) {
    VkPipelineVertexInputStateCreateInfo vertex_input_state = {VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO};
    
    vertex_input_state.pNext = NULL;
    vertex_input_state.flags = 0;
    vertex_input_state.vertexBindingDescriptionCount = vertex_binding_description_count;
    vertex_input_state.pVertexBindingDescriptions = pvertex_binding_descriptions;
    vertex_input_state.vertexAttributeDescriptionCount = vertex_attribute_description_count;
    vertex_input_state.pVertexAttributeDescriptions = pvertex_attribute_descriptions;
    
    return vertex_input_state;
};

VkPipelineColorBlendAttachmentState create_color_blend_attach_state(VkBool32 blend_enable,  VkBlendFactor src_colour_blend_factor,
VkBlendFactor dst_colour_blend_factor, VkBlendOp colour_blend_op, VkBlendFactor src_alpha_blend_factor, 
VkBlendFactor dst_alpha_blend_factor, VkBlendOp alpha_blend_op,  VkColorComponentFlags colour_write_mask) {
    VkPipelineColorBlendAttachmentState colour_blend_attachment = {};
    colour_blend_attachment.blendEnable = blend_enable;
    colour_blend_attachment.srcColorBlendFactor = src_colour_blend_factor;
    colour_blend_attachment.dstColorBlendFactor = dst_colour_blend_factor;
    colour_blend_attachment.colorBlendOp = colour_blend_op;
    colour_blend_attachment.srcAlphaBlendFactor = src_alpha_blend_factor;
    colour_blend_attachment.dstAlphaBlendFactor = dst_alpha_blend_factor;
    colour_blend_attachment.alphaBlendOp = alpha_blend_op;
    colour_blend_attachment.colorWriteMask = colour_write_mask;

    return colour_blend_attachment;
}

VkPipelineColorBlendStateCreateInfo create_colour_blend_state(VkBool32 logic_op_enable, VkLogicOp logic_op, 
uint32_t attachment_count, const VkPipelineColorBlendAttachmentState *p_attachments) {
    VkPipelineColorBlendStateCreateInfo colour_blend_state = {VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO};
     colour_blend_state.pNext = NULL;
     colour_blend_state.flags = 0;
     colour_blend_state.logicOpEnable = logic_op_enable;
     colour_blend_state.logicOp = logic_op;
    colour_blend_state.attachmentCount = attachment_count;
    colour_blend_state.pAttachments = p_attachments;
    for(uint32_t i=0;i<4;i++){
		colour_blend_state.blendConstants[i]=0.0f;
    }
    return colour_blend_state;
}

VkPipelineInputAssemblyStateCreateInfo create_input_assembly(VkPrimitiveTopology input_assembly_topology) {
    VkPipelineInputAssemblyStateCreateInfo input_assembly = {VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO};
     input_assembly.pNext = NULL;
     input_assembly.flags = 0;
    input_assembly.topology = input_assembly_topology;
     input_assembly.primitiveRestartEnable = VK_FALSE;

    return input_assembly;
}

VkPipelineViewportStateCreateInfo create_viewport_state_info(uint32_t viewport_count, const VkViewport *view_port, uint32_t scissor_count, const VkRect2D *scissor) {
    VkPipelineViewportStateCreateInfo viewport_state = {VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO};
     viewport_state.pNext = NULL;
     viewport_state.flags = 0;
    viewport_state.viewportCount = viewport_count;
    viewport_state.pViewports = view_port;
    viewport_state.scissorCount = scissor_count;
    viewport_state.pScissors = scissor;

    return viewport_state;
}

VkPipelineRasterizationStateCreateInfo create_rasterization_state_info(VkBool32 depth_clamp_enable, VkBool32 rasterizer_discard_enable,
    VkPolygonMode polygon_mode, VkCullModeFlags cull_mode, VkFrontFace front_face, VkBool32 depth_bias_enable, float depth_bias_constant_factor,
    float depth_bias_clamp, float depth_bias_slope_factor, float line_width) {
    VkPipelineRasterizationStateCreateInfo rasterization_state = {VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO};
    rasterization_state.pNext = NULL;
    rasterization_state.flags = 0;
    rasterization_state.depthClampEnable = depth_clamp_enable;
    rasterization_state.rasterizerDiscardEnable = rasterizer_discard_enable;
    rasterization_state.polygonMode = polygon_mode;
    rasterization_state.cullMode = cull_mode;
    rasterization_state.frontFace = front_face;
    rasterization_state.depthBiasEnable = depth_bias_enable;
    rasterization_state.depthBiasConstantFactor = depth_bias_constant_factor;
    rasterization_state.depthBiasClamp = depth_bias_clamp;
    rasterization_state.depthBiasSlopeFactor = depth_bias_slope_factor;
    rasterization_state.lineWidth = line_width;

    return rasterization_state;
}

VkPipelineMultisampleStateCreateInfo create_multisample_state_info(VkSampleCountFlagBits rasterization_samples, VkBool32 sample_shading_enable, 
float min_sample_shading, const VkSampleMask * sample_mask, VkBool32 alpha_to_coverage_enable, VkBool32 alpha_to_one_enable) {
    VkPipelineMultisampleStateCreateInfo multi_sample_state = {VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO};
     multi_sample_state.pNext = NULL;
     multi_sample_state.flags = 0;
    multi_sample_state.rasterizationSamples = rasterization_samples;
     multi_sample_state.sampleShadingEnable = sample_shading_enable;
     multi_sample_state.minSampleShading = min_sample_shading;
     multi_sample_state.pSampleMask = sample_mask;
     multi_sample_state.alphaToCoverageEnable = alpha_to_coverage_enable;
     multi_sample_state.alphaToOneEnable = alpha_to_one_enable;

    return multi_sample_state;
}

VkCommandPoolCreateInfo create_command_pool_info( uint32_t queue_family_index) {
    VkCommandPoolCreateInfo command_pool_info = {VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO};
    command_pool_info.pNext = NULL;
    command_pool_info.flags = 0;
    command_pool_info.queueFamilyIndex = queue_family_index;

    return command_pool_info;
};

VkViewport create_viewport(float viewport_x, float viewport_y , float viewport_width, float viewport_height, float viewport_min_depth, float viewport_max_height){

    VkViewport viewport;

    viewport.x = viewport_x;
    viewport.y = viewport_y;
    viewport.width  = viewport_width;
    viewport.height = viewport_height;
    viewport.minDepth = viewport_min_depth;
    viewport.maxDepth = viewport_min_depth;

    return viewport;

}

VkPipelineLayoutCreateInfo create_pipeline_layout_info(uint32_t set_layout_count, const VkDescriptorSetLayout * pset_layouts, uint32_t push_constant_range_count,
const VkPushConstantRange * ppush_constant_ranges){

VkPipelineLayoutCreateInfo pipeline_layout_create_info = {VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO};
pipeline_layout_create_info.pNext = NULL;
pipeline_layout_create_info.flags = 0;
pipeline_layout_create_info.setLayoutCount = set_layout_count;
pipeline_layout_create_info.pSetLayouts = pset_layouts;
pipeline_layout_create_info.pushConstantRangeCount = push_constant_range_count;
pipeline_layout_create_info.pPushConstantRanges = ppush_constant_ranges;

return pipeline_layout_create_info;

}

VkGraphicsPipelineCreateInfo  create_graphics_pipeline_info(uint32_t stage_count, const VkPipelineShaderStageCreateInfo * p_stages, 
const VkPipelineVertexInputStateCreateInfo* pvertex_input_state, const VkPipelineInputAssemblyStateCreateInfo* pinput_assembly_state,
 const VkPipelineTessellationStateCreateInfo* ptessellation_state,   const VkPipelineViewportStateCreateInfo*  pviewport_state, 
 const VkPipelineRasterizationStateCreateInfo*  prasterization_state,  const VkPipelineMultisampleStateCreateInfo* pmultisample_state, 
   const VkPipelineDepthStencilStateCreateInfo* pdepth_stencil_state, const VkPipelineColorBlendStateCreateInfo*   pcolour_blend_state, 
  const VkPipelineDynamicStateCreateInfo*   pdynamic_state, VkPipelineLayout pipe_layout, VkRenderPass pipeline_renderpass, 
  uint32_t pipeline_subpass, VkPipeline base_pipeline_handle, int32_t base_pipeline_index

 ){

    VkGraphicsPipelineCreateInfo graphics_pipeline_create_info = {VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO};
    graphics_pipeline_create_info.pNext = NULL;
    graphics_pipeline_create_info.flags = 0;// VK_PIPELINE_CREATE_DERIVATIVE_BIT;
    graphics_pipeline_create_info.stageCount = stage_count;
    graphics_pipeline_create_info.pStages = p_stages;
    graphics_pipeline_create_info.pVertexInputState = pvertex_input_state; 
    graphics_pipeline_create_info.pInputAssemblyState = pinput_assembly_state;
      graphics_pipeline_create_info.pTessellationState = ptessellation_state;
      graphics_pipeline_create_info.pViewportState = pviewport_state;
     graphics_pipeline_create_info. pRasterizationState = prasterization_state;
        graphics_pipeline_create_info. pMultisampleState = pmultisample_state;
     graphics_pipeline_create_info.pDepthStencilState = pdepth_stencil_state;
        graphics_pipeline_create_info.pColorBlendState = pcolour_blend_state;
         graphics_pipeline_create_info.pDynamicState = pdynamic_state;
   graphics_pipeline_create_info.layout = pipe_layout;
      graphics_pipeline_create_info.renderPass = pipeline_renderpass;
  graphics_pipeline_create_info.subpass = pipeline_subpass;
    graphics_pipeline_create_info.basePipelineHandle = base_pipeline_handle;
  
    graphics_pipeline_create_info.basePipelineIndex = base_pipeline_index;

    return graphics_pipeline_create_info;
}



#define CREATE_INSTANCE_FUNC(name) name = (PFN_##name)vkGetInstanceProcAddr(vk_context.instance, #name)

#define CREATE_DEVICE_FUNC(name) name = (PFN_##name)vkGetDeviceProcAddr(vk_context.logical_device, #name)

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
                                                                                                    void load_global_level_functions() {
    vulkan_lib = dlopen("libvulkan.so", RTLD_NOW);

    vkGetInstanceProcAddr = (PFN_vkGetInstanceProcAddr)dlsym(vulkan_lib, "vkGetInstanceProcAddr");
    vkDestroyInstance = (PFN_vkDestroyInstance)dlsym(vulkan_lib, "vkDestroyInstance");
    vkCreateInstance = (PFN_vkCreateInstance)vkGetInstanceProcAddr(vk_context.instance, "vkCreateInstance");
};

void load_instance_level_funcs() {
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
}

void load_device_level_funcs() {
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
}

int main() {
    load_global_level_functions();
    vk_context.allocator = 0;
    VkApplicationInfo app_info = create_app_info("Learn", VK_MAKE_VERSION(1, 0, 0), "test", VK_MAKE_VERSION(1, 0, 0), VK_API_VERSION_1_0);

    const char *extensions[] = {VK_KHR_SURFACE_EXTENSION_NAME, VK_KHR_XCB_SURFACE_EXTENSION_NAME, VK_EXT_DEBUG_UTILS_EXTENSION_NAME};
    const char *layers[] = {"VK_LAYER_KHRONOS_validation"};

    VkInstanceCreateInfo instance_info = create_instance_info(&app_info, ArraySize(layers), layers, ArraySize(extensions), extensions);

    VK_CHECK(vkCreateInstance(&instance_info, vk_context.allocator, &vk_context.instance));

    load_instance_level_funcs();
    // enumerate physical devices and store in in phy dev count.
    uint32_t physical_device_count = 0;
    VK_CHECK(vkEnumeratePhysicalDevices(vk_context.instance, &physical_device_count, NULL));
    // create an array of VkPhysicalDevices of the size returned by the first enumeration.
    VkPhysicalDevice physical_devices[physical_device_count];
    // enumerate again, and we now have an array of physical devies two be queried.
    VK_CHECK(vkEnumeratePhysicalDevices(vk_context.instance, &physical_device_count, physical_devices));

    VkPhysicalDeviceProperties physical_device_properties[physical_device_count];

    uint32_t discrete_gpu_list[physical_device_count];
    uint32_t discrete_gpu_count = 0;
    uint32_t integrated_gpu_list[physical_device_count];
    uint32_t integrated_gpu_count = 0;

    VkPhysicalDeviceMemoryProperties physical_device_memory_properties[physical_device_count];
    uint32_t physical_device_memory_count[physical_device_count];
    VkDeviceSize physical_device_memory_total[physical_device_count];

    for (uint32_t i = 0; i < physical_device_count; i++) {
        vkGetPhysicalDeviceProperties(physical_devices[i], &physical_device_properties[i]);
        if (physical_device_properties[i].deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
            discrete_gpu_list[discrete_gpu_count] = i;
            discrete_gpu_count++;
        } else if (physical_device_properties[i].deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU) {
            integrated_gpu_list[integrated_gpu_count] = i;
            integrated_gpu_count++;
        };

        vkGetPhysicalDeviceMemoryProperties(physical_devices[i], &physical_device_memory_properties[i]);
        physical_device_memory_count[i] = physical_device_memory_properties[i].memoryHeapCount;
        physical_device_memory_total[i] = 0;
        for (uint32_t j = 0; j < physical_device_memory_count[i]; j++) {
            physical_device_memory_total[i] += physical_device_memory_properties[i].memoryHeaps[j].size;
        }
    }

    VkDeviceSize max_memory_size = 0;
    uint32_t physical_device_best_index = 0;

    if (discrete_gpu_count != 0) {
        for (uint32_t i = 0; i < discrete_gpu_count; i++) {
            if (physical_device_memory_total[i] > max_memory_size) {
                physical_device_best_index = discrete_gpu_list[i];
                max_memory_size = physical_device_memory_total[i];
            }
        }
    } else if (integrated_gpu_count != 0) {
        for (uint32_t i = 0; i < integrated_gpu_count; i++) {
            if (physical_device_memory_total[i] > max_memory_size) {
                physical_device_best_index = integrated_gpu_list[i];
                max_memory_size = physical_device_memory_total[i];
            }
        }
    }

    printf("Best device index: %u\n", physical_device_best_index);
    printf("Device name :%s\n", physical_device_properties[physical_device_best_index].deviceName);
    printf("Device type:");
    if (discrete_gpu_count != 0) {
        printf("Discrete gpu\n");
    } else if (integrated_gpu_count != 0) {
        printf("Integrated gpu\n");
    } else {
        printf("unknown");
    };

    const uint64_t mib = 1024 * 1024;

    printf("memory total in mibiytes:%lu\n", physical_device_memory_total[physical_device_best_index] / mib);
    VkPhysicalDevice *physical_device_best_device = &(physical_devices[physical_device_best_index]);

    uint32_t queue_families_properties_count = 0;

    vkGetPhysicalDeviceQueueFamilyProperties(*physical_device_best_device, &queue_families_properties_count, NULL);
    VkQueueFamilyProperties queue_family_properties[queue_families_properties_count];
    vkGetPhysicalDeviceQueueFamilyProperties(*physical_device_best_device, &queue_families_properties_count, queue_family_properties);

    uint32_t queue_family_queue_count[queue_families_properties_count];
    for (uint32_t i = 0; i < queue_families_properties_count; i++) {
        queue_family_queue_count[i] = queue_family_properties[i].queueCount;
    }

    VkDeviceQueueCreateInfo device_queue_create_infos[queue_families_properties_count];
    for (uint32_t i = 0; i < queue_families_properties_count; i++) {
        device_queue_create_infos[i].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        device_queue_create_infos[i].pNext = NULL;
        device_queue_create_infos[i].flags = 0;
        device_queue_create_infos[i].queueFamilyIndex = i;
        device_queue_create_infos[i].queueCount = queue_family_queue_count[i];
        float queue_priority[1] = {1.0f};
        device_queue_create_infos[i].pQueuePriorities = queue_priority;
    }

    printf("using %d queue families\n", queue_families_properties_count);

    const char *required_extension = VK_KHR_SWAPCHAIN_EXTENSION_NAME;
    VkPhysicalDeviceFeatures physical_device_features;
    vkGetPhysicalDeviceFeatures(*physical_device_best_device, &physical_device_features);

    VkDeviceCreateInfo device_create_info = create_device_info(queue_families_properties_count, device_queue_create_infos, 1, &required_extension, &physical_device_features);

    VK_CHECK(vkCreateDevice(*physical_device_best_device, &device_create_info, vk_context.allocator, &vk_context.logical_device));
    load_device_level_funcs();

    uint32_t queue_family_graphics_count = 0;
    uint32_t queue_family_graphics_list[queue_family_graphics_count];
    for (uint32_t i = 0; i < queue_families_properties_count; i++) {
        if ((queue_family_properties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0) {
            queue_family_graphics_list[queue_family_graphics_count] = i;
            queue_family_graphics_count++;
        }
    }

    uint32_t max_queue_count = 0;
    uint32_t queue_family_best_queue_index = 0;
    for (uint32_t i = 0; i < queue_family_graphics_count; i++) {
        if (queue_family_properties->queueCount > max_queue_count) {
            queue_family_best_queue_index = queue_family_graphics_list[i];
        }
    }

    printf("best queue family index %d\n", queue_family_best_queue_index);

    VkQueue graphics_queue, present_queue;
    vkGetDeviceQueue(vk_context.logical_device, queue_family_best_queue_index, 0, &present_queue);
    char single_queue = 1;
    if (queue_family_properties[queue_family_best_queue_index].queueCount < 2) {
        vkGetDeviceQueue(vk_context.logical_device, queue_family_best_queue_index, 0, &present_queue);
        printf("using single queue for drawing\n");
    } else {
        single_queue = 0;
        vkGetDeviceQueue(vk_context.logical_device, queue_family_best_queue_index, 1, &present_queue);
        printf("using double queues for rawing");
    };
    init_xwindow();
    VkXcbSurfaceCreateInfoKHR surface_create_info = create_xcb_surface(xcb_context.connection, xcb_context.window);

    vkCreateXcbSurfaceKHR(vk_context.instance, &surface_create_info, vk_context.allocator, &vk_context.surface);
    VkBool32 surface_support;

    VK_CHECK(vkGetPhysicalDeviceSurfaceSupportKHR(*physical_device_best_device, queue_family_best_queue_index, vk_context.surface, &surface_support));
   // printf("%d, %d", xcb_context.WINDOW_WIDTH, xcb_context.WINDOW_HEIGHT);

    VkSurfaceCapabilitiesKHR surface_capabilities;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(*physical_device_best_device, vk_context.surface, &surface_capabilities);
    //printf("%u, %u", surface_capabilities.currentExtent, surface_capabilities.maxImageExtent);

    char extent_suitable = 1;
    int window_width, window_height;

    /*VkExtent2D actual_extent;
    actual_extent.width = window_width;
    actual_extent.height = window_height;
    if(surface_capabilities.currentExtent.width != window_width || surface_capabilities.currentExtent.height != window_height){

        extent_suitable = 0;
        printf("actual extent size doesnt match framebuffers, resizing");
        actual_extent.width=window_width >surface_capabilities.maxImageExtent.width? surface_capabilities.maxImageExtent.width:window_width;
                    actual_extent.width=window_width<surface_capabilities.minImageExtent.width?surface_capabilities.minImageExtent.width:window_width;
                    actual_extent.height=window_height>surface_capabilities.maxImageExtent.height?surface_capabilities.maxImageExtent.height:window_height;
                    actual_extent.height=window_height<surface_capabilities.minImageExtent.height?surface_capabilities.minImageExtent.height:window_height;

    }
    */

    uint32_t surface_format_count;
    vkGetPhysicalDeviceSurfaceFormatsKHR(*physical_device_best_device, vk_context.surface, &surface_format_count, NULL);
    VkSurfaceFormatKHR surface_formats[surface_format_count];
    vkGetPhysicalDeviceSurfaceFormatsKHR(*physical_device_best_device, vk_context.surface, &surface_format_count, surface_formats);

    uint32_t present_mode_count;

    vkGetPhysicalDeviceSurfacePresentModesKHR(*physical_device_best_device, vk_context.surface, &present_mode_count, NULL);

    VkPresentModeKHR present_modes[present_mode_count];
    vkGetPhysicalDeviceSurfacePresentModesKHR(*physical_device_best_device, vk_context.surface, &present_mode_count, present_modes);

    char mailbox_mode_supported = 0;
    for (uint32_t i = 0; i < present_mode_count; i++) {
    };

    uint32_t queue_family_indices[2] = {0, 1};

    VkSwapchainCreateInfoKHR swap_info = create_swap_info(vk_context.surface, surface_capabilities.minImageCount + 1, surface_formats[0].format,
                                                          surface_formats[0].colorSpace, surface_capabilities.currentExtent, 1, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, single_queue ? VK_SHARING_MODE_EXCLUSIVE : VK_SHARING_MODE_CONCURRENT, single_queue ? 0 : 2,
                                                          single_queue ? NULL : queue_family_indices, surface_capabilities.currentTransform, VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR, mailbox_mode_supported ? VK_PRESENT_MODE_MAILBOX_KHR : VK_PRESENT_MODE_FIFO_KHR, VK_TRUE, VK_NULL_HANDLE);

    VK_CHECK(vkCreateSwapchainKHR(vk_context.logical_device, &swap_info, vk_context.allocator, &vk_context.swapchain));

    uint32_t swap_image_count;
    vkGetSwapchainImagesKHR(vk_context.logical_device, vk_context.swapchain, &swap_image_count, NULL);
    VkImage swap_images[swap_image_count];
    vkGetSwapchainImagesKHR(vk_context.logical_device, vk_context.swapchain, &swap_image_count, swap_images);

    VkImageView image_views[swap_image_count];
    VkImageViewCreateInfo image_view_create_info[swap_image_count];
    VkComponentMapping image_view_rba_comp;
    image_view_rba_comp.r = VK_COMPONENT_SWIZZLE_IDENTITY;
    image_view_rba_comp.g = VK_COMPONENT_SWIZZLE_IDENTITY;
    image_view_rba_comp.b = VK_COMPONENT_SWIZZLE_IDENTITY;
    image_view_rba_comp.a = VK_COMPONENT_SWIZZLE_IDENTITY;

    VkImageSubresourceRange image_view_subresource;
    image_view_subresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    image_view_subresource.baseMipLevel = 0;
    image_view_subresource.levelCount = 1;
    image_view_subresource.baseArrayLayer = 0;
    image_view_subresource.layerCount = swap_info.imageArrayLayers;

    for (uint32_t i = 0; i < swap_image_count; i++) {
        image_view_create_info[i].sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        image_view_create_info[i].pNext = NULL;
        image_view_create_info[i].flags = 0;
        image_view_create_info[i].image = swap_images[i];
        image_view_create_info[i].viewType = VK_IMAGE_VIEW_TYPE_2D;
        image_view_create_info[i].format = surface_formats[0].format;
        image_view_create_info[i].components = image_view_rba_comp;
        image_view_create_info[i].subresourceRange = image_view_subresource;
        VK_CHECK(vkCreateImageView(vk_context.logical_device, &image_view_create_info[i], NULL, &image_views[i]));
    };

    VkAttachmentDescription attachment_description = create_attachment_description(0, swap_info.imageFormat, VK_SAMPLE_COUNT_1_BIT, VK_ATTACHMENT_LOAD_OP_CLEAR,
                                                                                   VK_ATTACHMENT_STORE_OP_STORE, VK_ATTACHMENT_LOAD_OP_DONT_CARE, VK_ATTACHMENT_STORE_OP_STORE, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);

    VkAttachmentReference attachment_reference = create_attachment_reference(0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

    VkSubpassDescription subpass_description = create_subpass_desc(0, VK_PIPELINE_BIND_POINT_GRAPHICS, 0, NULL, 1, &attachment_reference, NULL, NULL, 0, NULL);

    VkSubpassDependency subpass_dependency;
    subpass_dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    subpass_dependency.dstSubpass = 0;
    subpass_dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    subpass_dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    subpass_dependency.srcAccessMask = 0;
    subpass_dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    subpass_dependency.dependencyFlags = 0;

    VkRenderPassCreateInfo renderpass_info = create_renderpass_info(NULL, 0, 1, &attachment_description, 1, &subpass_description, 1, &subpass_dependency);

    VK_CHECK(vkCreateRenderPass(vk_context.logical_device, &renderpass_info, vk_context.allocator, &vk_context.renderpass));

    FILE *vert_file = NULL, *frag_file = NULL;
    vert_file = fopen("vert.spv", "rb+");

    frag_file = fopen("frag.spv", "rb+");
    char shader_loaded = 1;
    if (vert_file == NULL || frag_file == NULL) {
        shader_loaded = 0;
        printf("could loadnt spv files");
    };
    fseek(vert_file, 0, SEEK_END);
    fseek(frag_file, 0, SEEK_END);

    uint32_t vert_size = ftell(vert_file);
    uint32_t frag_size = ftell(frag_file);

    char *vert_code = (char *)malloc(vert_size * sizeof(char));
    char *frag_code = (char *)malloc(frag_size * sizeof(char));

    rewind(vert_file);
    rewind(frag_file);
    fread(vert_code, 1, vert_size, vert_file);
    fread(frag_code, 1, frag_size, frag_file);

    fclose(vert_file);
    fclose(frag_file);

    VkShaderModuleCreateInfo frag_module_create_info;
    frag_module_create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    frag_module_create_info.pNext = NULL;
    frag_module_create_info.flags = 0;
    frag_module_create_info.codeSize = shader_loaded ? frag_size : 0;
    frag_module_create_info.pCode = shader_loaded ? (const uint32_t *)frag_code : NULL;

    VkShaderModuleCreateInfo vert_module_create_info;
    vert_module_create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    vert_module_create_info.pNext = NULL;
    vert_module_create_info.flags = 0;
    vert_module_create_info.codeSize = shader_loaded ? vert_size : 0;
    vert_module_create_info.pCode = shader_loaded ? (const uint32_t *)vert_code : NULL;

    VkShaderModule vert_shader_module, frag_shader_module;
    vkCreateShaderModule(vk_context.logical_device, &vert_module_create_info, vk_context.allocator, &vert_shader_module);
vkCreateShaderModule(vk_context.logical_device, &frag_module_create_info, vk_context.allocator, &frag_shader_module);

VkPipelineShaderStageCreateInfo vert_stage_create_info;
 VkPipelineShaderStageCreateInfo frag_stage_create_info;
 VkPipelineShaderStageCreateInfo   shader_stage_create_infos[2];

vert_stage_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
vert_stage_create_info.pNext = NULL;
vert_stage_create_info.flags = 0;
vert_stage_create_info.stage = VK_SHADER_STAGE_VERTEX_BIT;
vert_stage_create_info.module = vert_shader_module;
char vertex_entry[VK_MAX_EXTENSION_NAME_SIZE];
strcpy(vertex_entry, "main");
vert_stage_create_info.pName = vertex_entry;
vert_stage_create_info.pSpecializationInfo =NULL;

frag_stage_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
frag_stage_create_info.pNext = NULL;
frag_stage_create_info.flags = 0;
frag_stage_create_info.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
frag_stage_create_info.module = frag_shader_module;
char fragment_entry[VK_MAX_EXTENSION_NAME_SIZE];
strcpy(fragment_entry, "main");
frag_stage_create_info.pName = fragment_entry;
frag_stage_create_info.pSpecializationInfo =NULL;

shader_stage_create_infos[0] = vert_stage_create_info;
shader_stage_create_infos[1] = frag_stage_create_info;

VkPipelineVertexInputStateCreateInfo vertex_input_info = create_vertex_input_state(0, NULL, 0, NULL);

VkPipelineInputAssemblyStateCreateInfo input_assembly_state_create_info = create_input_assembly(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);

VkViewport viewport = create_viewport(0.0f, 0.0f, (float)swap_info.imageExtent.width, (float)swap_info.imageExtent.height, 0.0f, 1.0f);

VkRect2D scissor;

VkOffset2D scissor_offset;

scissor_offset.x=0;
scissor_offset.y=0;
scissor.offset=scissor_offset;
scissor.extent=swap_info.imageExtent;

VkPipelineViewportStateCreateInfo viewport_state_info = create_viewport_state_info(1, &viewport, 1, &scissor);

VkPipelineRasterizationStateCreateInfo rasterization_state_create_info = create_rasterization_state_info(VK_FALSE, VK_FALSE, VK_POLYGON_MODE_FILL, 
VK_CULL_MODE_BACK_BIT, VK_FRONT_FACE_CLOCKWISE, VK_FALSE, 0.0f, 0.0f, 0.0f, 1.0f);

VkPipelineMultisampleStateCreateInfo multisample_state_create_info = create_multisample_state_info(VK_SAMPLE_COUNT_1_BIT, VK_FALSE, 1.0f, NULL, VK_FALSE, 
VK_FALSE);

VkPipelineColorBlendAttachmentState color_blend_attachment_state = create_color_blend_attach_state(VK_FALSE, VK_BLEND_FACTOR_ONE, VK_BLEND_FACTOR_ZERO, 
VK_BLEND_OP_ADD, VK_BLEND_FACTOR_ONE, VK_BLEND_FACTOR_ZERO, VK_BLEND_OP_ADD, VK_COLOR_COMPONENT_R_BIT
		|VK_COLOR_COMPONENT_G_BIT
		|VK_COLOR_COMPONENT_B_BIT
		|VK_COLOR_COMPONENT_A_BIT);

        VkPipelineColorBlendStateCreateInfo colour_blend_state_info = create_colour_blend_state(VK_FALSE, VK_LOGIC_OP_COPY, 1, &color_blend_attachment_state);

VkPipelineLayoutCreateInfo pipeline_layout_create_info = create_pipeline_layout_info(0, NULL, 0, NULL);

VkPipelineLayout pipeline_layout;
VK_CHECK(vkCreatePipelineLayout(vk_context.logical_device, &pipeline_layout_create_info, vk_context.allocator, &pipeline_layout));

VkGraphicsPipelineCreateInfo graphics_pipeline_create_info = create_graphics_pipeline_info(2, shader_stage_create_infos, &vertex_input_info, 
&input_assembly_state_create_info, NULL, &viewport_state_info, &rasterization_state_create_info, &multisample_state_create_info, 
NULL, &colour_blend_state_info, 
NULL, pipeline_layout, vk_context.renderpass, 0, VK_NULL_HANDLE, -1 );
VkPipeline pipeline;
VK_CHECK(vkCreateGraphicsPipelines(vk_context.logical_device, VK_NULL_HANDLE, 1, &graphics_pipeline_create_info, vk_context.allocator, &pipeline));

vkDestroyShaderModule(vk_context.logical_device, frag_shader_module, vk_context.allocator);
vkDestroyShaderModule(vk_context.logical_device, vert_shader_module, vk_context.allocator);
free(vert_code);
free(frag_code);

VkFramebufferCreateInfo framebuffer_create_info[swap_image_count];
VkFramebuffer frame_buffers[swap_image_count];
VkImageView image_attachments[swap_image_count];
for(uint32_t i = 0; i < swap_image_count; i++){

    image_attachments[i] = image_views[i];
    framebuffer_create_info[i] = create_framebuffer_info(vk_context.renderpass, 1, &(image_attachments[i]), swap_info.imageExtent.width, 
    swap_info.imageExtent.height, 1);
    vkCreateFramebuffer(vk_context.logical_device, &(framebuffer_create_info[i]), vk_context.allocator, &(frame_buffers[i]));
};

VkCommandPoolCreateInfo command_pool_create_info = create_command_pool_info( queue_family_best_queue_index);

VkCommandPool command_pool;
vkCreateCommandPool(vk_context.logical_device, &command_pool_create_info, NULL, &command_pool);

VkCommandBufferAllocateInfo command_buffer_alloc_info = cmd_alloc_info(command_pool, VK_COMMAND_BUFFER_LEVEL_PRIMARY, swap_image_count);

VkCommandBuffer command_buffers[swap_image_count];
VK_CHECK(vkAllocateCommandBuffers(vk_context.logical_device, &command_buffer_alloc_info, command_buffers));

//VkCommandBufferBeginInfo command_buffer_begin_info[swap_image_count] = cmd_begin_info()
VkCommandBufferBeginInfo command_buffer_begin_info[swap_image_count];
VkRenderPassBeginInfo renderpass_begin_info[swap_image_count];
VkRect2D renderpass_area;
renderpass_area.offset.x = 0;
renderpass_area.offset.y = 0;
renderpass_area.extent = swap_info.imageExtent;
VkClearValue clear_val = {0.6f, 0.2f, 0.8f, 0.0f};
for(uint32_t i = 0; i < swap_image_count; i++){

    command_buffer_begin_info[i].sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    command_buffer_begin_info[i].pNext = NULL;
    command_buffer_begin_info[i].flags = 0;
     command_buffer_begin_info[i].pInheritanceInfo =NULL;
     
     renderpass_begin_info[i].sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
     renderpass_begin_info[i].pNext = NULL;
     renderpass_begin_info[i].renderPass = vk_context.renderpass;
     renderpass_begin_info[i].framebuffer = frame_buffers[i];
     renderpass_begin_info[i].renderArea = renderpass_area;
     renderpass_begin_info[i].clearValueCount = 1;
     renderpass_begin_info[i].pClearValues = &clear_val;

     vkBeginCommandBuffer(command_buffers[i], &command_buffer_begin_info[i]);
     vkCmdBeginRenderPass(command_buffers[i], &(renderpass_begin_info[i]), VK_SUBPASS_CONTENTS_INLINE);

     vkCmdBindPipeline(command_buffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
     vkCmdEndRenderPass(command_buffers[i]);
     vkEndCommandBuffer(command_buffers[i]);
     

}

xwindow_main_loop();

    // notes: switch context->allocator, do macro madness for loading functions, = done. maybe mka e afucntion loader with cmjratori switch?
    // and fix device selection.
    // may need to double check stuff like vkimages[5]- are we oing imges? ie check up to video 21, not after
    // double imageviews. double check everything
    // do 600 lines one day, duble check the nexxt/ clean up codebase/make framework;
    // maybe repalce kohi stuff?
    // test thatvalidation layers are woorking- done
    // use renderdoc
}
