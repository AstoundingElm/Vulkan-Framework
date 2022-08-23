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

#define global_variable static
#define internal static
#define local_persist static

#define LoadFunction dlsym  

global_variable void * vulkan_library;

#define GLOBAL_VULKAN_FUNCTION_BUILDER(name) PFN_##name name; name = (PFN_##name)LoadFunction( vulkan_library, #name ); 
                                                                           
#define INSTANCE_VULKAN_FUNCTION_BUILDER( instance, name) PFN_##name name; name = (PFN_##name)vkGetInstanceProcAddr( instance, #name );      

#define DEVICE_VULKAN_FUNCTION_BUILDER(device, name) PFN_##name name; name = (PFN_##name)vkGetDeviceProcAddr( device, #name );  
                                                                                                                           
#define ArraySize(arr) sizeof((arr)) / sizeof((arr[0]))

#define VK_CHECK(result)                         \
    if (result != VK_SUCCESS)                    \
    {                                            \
        printf("Vulkan Error: %d", result); \
        __builtin_trap();                          \
    }

struct vulkan_types{
    VkInstance instance;
    VkSurfaceKHR surface;
    VkDebugUtilsMessengerEXT debug_messenger;
    int  graphics_index;
    VkPhysicalDevice struct_gpu;
    VkDevice logical_device;
    VkQueue graphics_queue;
    VkSurfaceFormatKHR surface_format;
    VkSwapchainKHR swapchain;
    uint32_t swap_img_count;
    VkImage sc_images[5];
    VkImageView swap_image_views[5];
    VkAllocationCallbacks * allocator;
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
};

struct xcb_types{

   Display* display;
    xcb_connection_t* connection;
    xcb_window_t window;
    xcb_screen_t* screen;
    xcb_atom_t wm_protocols;
    xcb_atom_t wm_delete_win;
    uint32_t WINDOW_WIDTH = 800;
    uint32_t WINDOW_HEIGHT = 600;
    bool running = false;

};

VKAPI_ATTR VkBool32 VKAPI_CALL vk_debug_callback(
    VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
    VkDebugUtilsMessageTypeFlagsEXT message_types,
    const VkDebugUtilsMessengerCallbackDataEXT* callback_data,
    void* user_data) {
    switch (message_severity) {
        default:
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
            printf("%s",callback_data->pMessage);
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
            printf("%s",callback_data->pMessage);
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
            printf("%s",callback_data->pMessage);
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
            printf("%s", callback_data->pMessage);
            break;
    }
    return VK_FALSE;
};

void init_xwindow(xcb_types * xcb_info){

int screenp = 0;
 xcb_info->connection = xcb_connect(NULL, &screenp);
if (xcb_connection_has_error( xcb_info->connection))
  printf("Failed to connect to X server using XCB.");

  xcb_screen_iterator_t iter =
    xcb_setup_roots_iterator(xcb_get_setup( xcb_info->connection));

for (int s = screenp; s > 0; s--)
    xcb_screen_next(&iter);
     xcb_info->screen = iter.data;
     xcb_info->window = xcb_generate_id( xcb_info->connection);
    uint32_t eventMask = XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK;
    uint32_t valueList[] = {  xcb_info->screen->black_pixel, 0, XCB_EVENT_MASK_BUTTON_PRESS | XCB_EVENT_MASK_BUTTON_RELEASE |
                       XCB_EVENT_MASK_KEY_PRESS | XCB_EVENT_MASK_KEY_RELEASE |
                       XCB_EVENT_MASK_EXPOSURE | XCB_EVENT_MASK_POINTER_MOTION |
                       XCB_EVENT_MASK_STRUCTURE_NOTIFY };
    xcb_create_window(  xcb_info->connection, XCB_COPY_FROM_PARENT,  xcb_info->window,  xcb_info->screen->root, 0, 0, 
     xcb_info->WINDOW_HEIGHT,  xcb_info->WINDOW_WIDTH, 0,
  XCB_WINDOW_CLASS_INPUT_OUTPUT,  xcb_info->screen->root_visual, eventMask, valueList);
const char * applicationName = "Hlll";
  xcb_change_property(
   xcb_info->connection,
  XCB_PROP_MODE_REPLACE,
   xcb_info->window,
  XCB_ATOM_WM_NAME,
  XCB_ATOM_STRING,
  8,
  strlen(applicationName),
  applicationName);
  
xcb_map_window( xcb_info->connection,  xcb_info->window);
xcb_flush( xcb_info->connection);
xcb_intern_atom_cookie_t wmDeleteCookie = xcb_intern_atom(
     xcb_info->connection, 0, strlen("WM_DELETE_WINDOW"), "WM_DELETE_WINDOW");
    
xcb_intern_atom_cookie_t wmProtocolsCookie =
    xcb_intern_atom( xcb_info->connection, 0, strlen("WM_PROTOCOLS"), "WM_PROTOCOLS");
xcb_intern_atom_reply_t *wmDeleteReply =
    xcb_intern_atom_reply( xcb_info->connection, wmDeleteCookie, NULL);
xcb_intern_atom_reply_t *wmProtocolsReply =
    xcb_intern_atom_reply( xcb_info->connection, wmProtocolsCookie, NULL);
 xcb_info->wm_delete_win = wmDeleteReply->atom;

 xcb_info->wm_protocols = wmProtocolsReply->atom;

xcb_change_property( xcb_info->connection, XCB_PROP_MODE_REPLACE,  xcb_info->window,
                    wmProtocolsReply->atom, 4, 32, 1, &wmDeleteReply->atom);
                     xcb_map_window( xcb_info->connection,  xcb_info->window);
  xcb_flush( xcb_info->connection);

};

void xwindow_main_loop(xcb_types * xcb_context){
  
  xcb_context->running = true;
  xcb_generic_event_t *event;
  xcb_client_message_event_t *cm;

  while (xcb_context->running) {
    event = xcb_wait_for_event( xcb_context->connection);

    switch (event->response_type & ~0x80) {
      case XCB_CLIENT_MESSAGE: {
        cm = (xcb_client_message_event_t *)event;

        if (cm->data.data32[0] ==  xcb_context->wm_delete_win)
           
          xcb_context->running = false;
          
        break;
        
      }
    }

    free(event);
  }

   xcb_destroy_window( xcb_context->connection,  xcb_context->window);

 };

 VkCommandBufferBeginInfo cmd_begin_info()
{
    VkCommandBufferBeginInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    return info;
};

VkFenceCreateInfo fence_create_info(VkFenceCreateFlags flags = 0)
{
    VkFenceCreateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    info.flags = flags;
    return info;
}

VkCommandBufferAllocateInfo cmd_alloc_info(VkCommandPool pool)
{
    VkCommandBufferAllocateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    info.commandBufferCount = 1;
    info.commandPool = pool;
    info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;

    return info;
}

VkApplicationInfo  create_app_info(const char * app_name, const char * engine_name, uint32_t api_version, uint32_t app_version, uint32_t engine_version ){

  VkApplicationInfo app_info = {VK_STRUCTURE_TYPE_APPLICATION_INFO, 0, app_name, app_version, engine_name, engine_version, api_version};
  return app_info;

};

VkInstanceCreateInfo create_instance_info(VkApplicationInfo * application_info,  uint32_t enabled_extension_count, 
const char * const *enabled_extensions_names, const char * const *enabled_layer_names, uint32_t enabled_layer_count){

  VkInstanceCreateInfo instance_create_info = {VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO, 0, 0, application_info, enabled_layer_count, enabled_layer_names,  enabled_extension_count,enabled_extensions_names};
  return instance_create_info;

};

VkXcbSurfaceCreateInfoKHR create_xcb_surface(xcb_connection_t * connection, xcb_window_t window){
VkXcbSurfaceCreateInfoKHR surface_create_info = {VK_STRUCTURE_TYPE_XCB_SURFACE_CREATE_INFO_KHR, 0, 0, connection, window};
return surface_create_info;

};

VkDebugUtilsMessengerCreateInfoEXT create_debug_info(VkDebugUtilsMessageSeverityFlagsEXT message_severity,VkDebugUtilsMessageTypeFlagsEXT message_type,
PFN_vkDebugUtilsMessengerCallbackEXT pfn_user_callback ){

  VkDebugUtilsMessengerCreateInfoEXT debug_create_info = {VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,0,0,
message_severity, message_type, pfn_user_callback};

return debug_create_info;
}

VkDeviceQueueCreateInfo create_queue_info(uint32_t queue_family_index, uint32_t queue_count, const float * queue_priorities){

  VkDeviceQueueCreateInfo queue_create_info = {VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO, 0, 0, queue_family_index, queue_count, queue_priorities};
  return queue_create_info;
}

VkDeviceCreateInfo create_device_info(uint32_t queue_create_info_count, const VkDeviceQueueCreateInfo * queue_create_infos, uint32_t enabled_layer_count,
const char * const * enabled_extensions_names){

VkDeviceCreateInfo device_create_info = {VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO, 0, 0, queue_create_info_count, queue_create_infos, enabled_layer_count,
enabled_extensions_names, 0};

return device_create_info;

};

VkSwapchainCreateInfoKHR create_swap_info(VkSurfaceKHR surface, uint32_t min_image_count, VkFormat image_format, VkExtent2D image_extent, 
uint32_t image_array_layers,VkImageUsageFlags image_usage, 
VkSurfaceTransformFlagBitsKHR pretransform, VkCompositeAlphaFlagBitsKHR composite_alpha){ 

  VkSwapchainCreateInfoKHR swap_info_create = {};
  
  swap_info_create.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
  //swap_info_create.pNext =
  //swap_info_create.flags =
  swap_info_create.surface = surface;
  swap_info_create.minImageCount = min_image_count;
  swap_info_create.imageFormat = image_format;
  //swap_info_create.imageColorSpace =
  swap_info_create.imageExtent = image_extent;
  swap_info_create.imageArrayLayers = image_array_layers;
  swap_info_create.imageUsage = image_usage;
  //swap_info_create.imageSharingMode =
  //swap_info_create.queueFamilyIndexCount =
  //swap_info_create.pQueueFamilyIndices =
  swap_info_create.preTransform = pretransform;
  swap_info_create.compositeAlpha = composite_alpha;
  //swap_info_create.presentMode =
  //swap_info_create.clipped =
  //swap_info_create.oldSwapchain =

return swap_info_create;

};

VkImageViewCreateInfo create_image_view_info(VkImage image, VkImageViewType viewtype, VkFormat format){

VkImageViewCreateInfo image_view_create_info = {};

  image_view_create_info.sType = {VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO};
  //image_view_create_info.pNext =
  image_view_create_info.image = image;
  image_view_create_info.viewType = viewtype;
  image_view_create_info.format = format;

  image_view_create_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  image_view_create_info.subresourceRange.levelCount = 1;
  image_view_create_info.subresourceRange.layerCount = 1;

  return image_view_create_info;
};

VkAttachmentDescription create_attachment_description(VkSampleCountFlagBits sample_count_bits, VkAttachmentLoadOp load_op, VkAttachmentStoreOp store_op, VkImageLayout
initial_layout, VkImageLayout final_layout){

VkAttachmentDescription colour_attachment;
//colour_attachment.flags = 
//colour_attachment.format = 
colour_attachment.samples = sample_count_bits;
colour_attachment.loadOp = load_op;
colour_attachment.storeOp = store_op;
//colour_attachment.stencilLoadOp =
//colour_attachment.stencilStoreOp = 
colour_attachment.initialLayout = initial_layout;
colour_attachment.finalLayout = final_layout;

return colour_attachment;
};

VkAttachmentReference create_attachment_reference(uint32_t attach, VkImageLayout attachment_reference_layout){

VkAttachmentReference attachment_reference = {};
attachment_reference.attachment = attach;
attachment_reference.layout = attachment_reference_layout;

return attachment_reference;

};

VkSubpassDescription create_subpass_desc(uint32_t colour_attachment_count, const VkAttachmentReference * colour_attach_ref){

VkSubpassDescription subpass_desc = {};
/*subpass_desc.flags =
subpass_desc.pipelineBindPoint =
subpass_desc.inputAttachmentCount =
subpass_desc.pInputAttachments =*/
subpass_desc.colorAttachmentCount = colour_attachment_count;
//subpass_desc.pColorAttachments =
//subpass_desc.pResolveAttachments =
subpass_desc.pDepthStencilAttachment = colour_attach_ref;
//subpass_desc.preserveAttachmentCount =
//subpass_desc.pPreserveAttachments =
return subpass_desc;

} 

VkRenderPassCreateInfo create_renderpass_info(uint32_t attachment_count, const VkAttachmentDescription * p_attachments, uint32_t subpass_count, 
const VkSubpassDescription * sub_passes  ){

  VkRenderPassCreateInfo renderpass_create_info = { VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO};
  //renderpass_create_info.pNext = 
  //renderpass_create_info.flags = 
  renderpass_create_info.attachmentCount = attachment_count;
  renderpass_create_info.pAttachments = p_attachments;
  renderpass_create_info.subpassCount = subpass_count;
  renderpass_create_info.pSubpasses = sub_passes;
  //renderpass_create_info.dependencyCount =
  //renderpass_create_info.pDependencies =

  return renderpass_create_info;

};

VkFramebufferCreateInfo create_framebuffer_info(VkRenderPass render_pass, uint32_t attachment_count, uint32_t ren_pass_width, uint32_t ren_pass_height,
uint32_t ren_pass_layers){

  VkFramebufferCreateInfo framebuffer_info = {VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO};
  //framebuffer_info.pNext =
 // framebuffer_info.flags =
  framebuffer_info.renderPass = render_pass;
  framebuffer_info.attachmentCount = attachment_count;
 // framebuffer_info.pAttachments =
  framebuffer_info.width = ren_pass_width;
  framebuffer_info.height = ren_pass_height;
  framebuffer_info.layers = ren_pass_layers;

  return framebuffer_info;
};

VkPipelineVertexInputStateCreateInfo create_vertex_input_state(){

  VkPipelineVertexInputStateCreateInfo vertex_input_state = {VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO};
  /*
  vertex_input_state.pNext = 
  vertex_input_state.flags = 
  vertex_input_state.vertexBindingDescriptionCount =
  vertex_input_state.pVertexBindingDescriptions =
  vertex_input_state.vertexAttributeDescriptionCount = 
  vertex_input_state.pVertexAttributeDescriptions =
  */
 return vertex_input_state;
};

VkPipelineColorBlendAttachmentState create_colour_blend_attachment(){

  VkPipelineColorBlendAttachmentState colour_blend_attachment = {VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO};
  colour_blend_attachment

}

/*
VkPipelineVertexInputStateCreateInfo vertex_input_state = {VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO};
VkPipelineColorBlendAttachmentState colour_blend_attachment = {};
colour_blend_attachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT  | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

colour_blend_attachment.blendEnable = VK_FALSE;
VkPipelineColorBlendStateCreateInfo colour_blend_state = {VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO};
colour_blend_state.pAttachments = &colour_blend_attachment;
colour_blend_state.attachmentCount = 1;

VkPipelineInputAssemblyStateCreateInfo input_assembly = {VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO};
input_assembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

VkViewport viewport = {};
viewport.maxDepth = 1.0;
VkRect2D scissor = {};

VkPipelineViewportStateCreateInfo viewport_state = {VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO};
viewport_state.pViewports = &viewport;
viewport_state.viewportCount = 1;
viewport_state.pScissors = &scissor;
viewport_state.scissorCount = 1;

VkPipelineRasterizationStateCreateInfo  rasterization_state = {VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO};
rasterization_state.cullMode = VK_CULL_MODE_BACK_BIT;
rasterization_state.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
rasterization_state.polygonMode = VK_POLYGON_MODE_FILL;
rasterization_state.lineWidth = 1.0f;

VkPipelineMultisampleStateCreateInfo multi_sample_state ={VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO};
multi_sample_state.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;*/

bool vk_init(vulkan_types * vk_context, xcb_types * xcb_context){

  vulkan_library = dlopen("libvulkan.so", RTLD_NOW); 

  vk_context->allocator = 0;

 GLOBAL_VULKAN_FUNCTION_BUILDER(vkGetInstanceProcAddr);

  VkApplicationInfo app_info = create_app_info("Petes engine", "Undefined", VK_API_VERSION_1_2, VK_MAKE_VERSION(1, 0, 0), VK_MAKE_VERSION(1, 0, 0) );

  const char * extensions[] = {VK_KHR_XCB_SURFACE_EXTENSION_NAME, VK_EXT_DEBUG_UTILS_EXTENSION_NAME, VK_KHR_SURFACE_EXTENSION_NAME};
  const char * layers[] = {"VK_LAYER_KHRONOS_validation"};

  VkInstanceCreateInfo create_info = create_instance_info(&app_info,  ArraySize(extensions), extensions, layers, ArraySize(layers));

INSTANCE_VULKAN_FUNCTION_BUILDER( vk_context->instance, vkCreateInstance);

  VK_CHECK(vkCreateInstance(&create_info, vk_context->allocator, &vk_context->instance));

 INSTANCE_VULKAN_FUNCTION_BUILDER(vk_context->instance, vkCreateDebugUtilsMessengerEXT );

VkDebugUtilsMessengerCreateInfoEXT debug_create_info = create_debug_info(
  VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT, 
  VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT, vk_debug_callback );

  VK_CHECK(vkCreateDebugUtilsMessengerEXT(vk_context->instance, &debug_create_info, vk_context->allocator, &vk_context->debug_messenger));

  VkXcbSurfaceCreateInfoKHR surface_create_info = create_xcb_surface(xcb_context->connection, xcb_context->window);
  
  INSTANCE_VULKAN_FUNCTION_BUILDER( vk_context->instance, vkCreateXcbSurfaceKHR);
  VkResult result = vkCreateXcbSurfaceKHR(vk_context->instance, &surface_create_info, vk_context->allocator, &vk_context->surface );
  
if(result != VK_SUCCESS){

  return 0;

};

vk_context->graphics_index = -1;
uint32_t gpu_count = 0;
VkPhysicalDevice gpus[10];

INSTANCE_VULKAN_FUNCTION_BUILDER(vk_context->instance, vkEnumeratePhysicalDevices);
VK_CHECK(vkEnumeratePhysicalDevices(vk_context->instance, &gpu_count, 0));
VK_CHECK(vkEnumeratePhysicalDevices(vk_context->instance, &gpu_count, gpus));

for(uint32_t i = 0; i < gpu_count; i++){

  VkPhysicalDevice gpu = gpus[i];
  uint32_t queue_family_count = 0;
  VkQueueFamilyProperties queue_props[10];

INSTANCE_VULKAN_FUNCTION_BUILDER(vk_context->instance, vkGetPhysicalDeviceQueueFamilyProperties);
  vkGetPhysicalDeviceQueueFamilyProperties(gpu, &queue_family_count, 0);
  vkGetPhysicalDeviceQueueFamilyProperties(gpu, &queue_family_count, queue_props);
  for(uint32_t j = 0; j < queue_family_count; j++){

    if(queue_props[j].queueFlags & VK_QUEUE_GRAPHICS_BIT & VK_QUEUE_TRANSFER_BIT){

      VkBool32 surface_support = VK_FALSE;

INSTANCE_VULKAN_FUNCTION_BUILDER(vk_context->instance, vkGetPhysicalDeviceSurfaceSupportKHR);
      VK_CHECK(vkGetPhysicalDeviceSurfaceSupportKHR(gpu, j, vk_context->surface, &surface_support));
      if(surface_support){

        vk_context->graphics_index = j;
        vk_context->struct_gpu = gpu;
        break;
      }
    }
  }
}

if(vk_context->graphics_index < 0){

  printf("couldnt find a graphics queue");
  return false;
  
}

float queue_priority = 1.0f;

VkDeviceQueueCreateInfo queue_info = create_queue_info(vk_context->graphics_index, 1, &queue_priority);

#define name "VK_KHR_portability_subset"
const char * swap_extensions[] = {name, VK_KHR_SWAPCHAIN_EXTENSION_NAME};

VkDeviceCreateInfo device_create_info = create_device_info(1, &queue_info, ArraySize(swap_extensions),swap_extensions);

INSTANCE_VULKAN_FUNCTION_BUILDER(vk_context->instance, vkCreateDevice);
VK_CHECK(vkCreateDevice(vk_context->struct_gpu, &device_create_info, vk_context->allocator, &vk_context->logical_device));

 // Get Graphics Queue

INSTANCE_VULKAN_FUNCTION_BUILDER(vk_context->instance, vkGetDeviceProcAddr);  
DEVICE_VULKAN_FUNCTION_BUILDER(vk_context->logical_device, vkGetDeviceQueue);

 vkGetDeviceQueue(vk_context->logical_device, vk_context->graphics_index, 0, &vk_context->graphics_queue);

uint32_t format_count = 0;
VkSurfaceFormatKHR surface_formats[10];

DEVICE_VULKAN_FUNCTION_BUILDER(vk_context->logical_device, vkGetPhysicalDeviceSurfaceFormatsKHR);
VK_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(vk_context->struct_gpu, vk_context->surface, &format_count, 0));
VK_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(vk_context->struct_gpu, vk_context->surface, &format_count, surface_formats));

for(uint32_t i = 0; i< format_count; i++){

  VkSurfaceFormatKHR format = surface_formats[i];
  if(format.format == VK_FORMAT_B8G8R8A8_SRGB){
vk_context->surface_format = format;
break;
}
}

VkSurfaceCapabilitiesKHR surface_caps;

INSTANCE_VULKAN_FUNCTION_BUILDER(vk_context->instance, vkGetPhysicalDeviceSurfaceCapabilitiesKHR);
VK_CHECK(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(vk_context->struct_gpu, vk_context->surface, &surface_caps));
uint32_t image_count = surface_caps.minImageCount +1;
image_count = image_count >surface_caps.maxImageCount ? image_count -1 :image_count;

VkSwapchainCreateInfoKHR swap_info = create_swap_info(vk_context->surface, image_count, vk_context->surface_format.format, surface_caps.currentExtent, 1, 
VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, surface_caps.currentTransform, VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR);

INSTANCE_VULKAN_FUNCTION_BUILDER(vk_context->instance, vkCreateSwapchainKHR);

VK_CHECK(vkCreateSwapchainKHR(vk_context->logical_device, &swap_info, vk_context->allocator, &vk_context->swapchain));

DEVICE_VULKAN_FUNCTION_BUILDER(vk_context->logical_device, vkGetSwapchainImagesKHR);
VK_CHECK(vkGetSwapchainImagesKHR(vk_context->logical_device, vk_context->swapchain, &vk_context->swap_img_count, 0));
VK_CHECK(vkGetSwapchainImagesKHR(vk_context->logical_device, vk_context->swapchain, &vk_context->swap_img_count, vk_context->sc_images));

DEVICE_VULKAN_FUNCTION_BUILDER(vk_context->logical_device, vkCreateImageView);

VkImageViewCreateInfo image_view_create_info = create_image_view_info(vk_context->test_image, VK_IMAGE_VIEW_TYPE_2D, vk_context->surface_format.format
);

  VK_CHECK(vkCreateImageView(vk_context->logical_device, &image_view_create_info, vk_context->allocator, &vk_context->test_image_view));


VkAttachmentDescription colour_attachment_description = create_attachment_description(VK_SAMPLE_COUNT_1_BIT, VK_ATTACHMENT_LOAD_OP_CLEAR, 
VK_ATTACHMENT_STORE_OP_STORE, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);

    VkAttachmentDescription attachments[] = {
            colour_attachment_description};


VkAttachmentReference colour_attachment_reference = create_attachment_reference(0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

VkSubpassDescription subpass_desc = create_subpass_desc(1, &colour_attachment_reference);

VkRenderPassCreateInfo renderpass_info = create_renderpass_info(ArraySize(attachments), attachments, 1, &subpass_desc);

DEVICE_VULKAN_FUNCTION_BUILDER(vk_context->logical_device, vkCreateRenderPass);
VK_CHECK(vkCreateRenderPass(vk_context->logical_device, &renderpass_info, vk_context->allocator, &vk_context->renderpass));

VkFramebufferCreateInfo framebuffer_info = create_framebuffer_info(vk_context->renderpass, 1, vk_context->screen_size.width, vk_context->screen_size.height, 1);

DEVICE_VULKAN_FUNCTION_BUILDER(vk_context->logical_device, vkCreateFramebuffer);

/* not a comment out VK_CHECK(vkCreateFramebuffer(vk_context->logical_device, &framebuffer_info, vk_context->allocator, &vk_context->framebuffers));*/

VkCommandPoolCreateInfo command_pool_info = create_command_pool()
 = {VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO};
command_pool_info.queueFamilyIndex = vk_context->graphics_index;
command_pool_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

 DEVICE_VULKAN_FUNCTION_BUILDER(vk_context->logical_device, vkCreateCommandPool);
VK_CHECK(vkCreateCommandPool(vk_context->logical_device, &command_pool_info, 0, &vk_context->command_pool));

VkCommandBufferAllocateInfo alloc_info = cmd_alloc_info(vk_context->command_pool);

DEVICE_VULKAN_FUNCTION_BUILDER(vk_context->logical_device, vkAllocateCommandBuffers);
VK_CHECK(vkAllocateCommandBuffers(vk_context->logical_device, &alloc_info, &vk_context->command_buffer));

VkSemaphoreCreateInfo semaphore_info = {VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO};

DEVICE_VULKAN_FUNCTION_BUILDER(vk_context->logical_device, vkCreateSemaphore);
VK_CHECK(vkCreateSemaphore(vk_context->logical_device, &semaphore_info, 0, &vk_context->aquire_semaphore));
VK_CHECK(vkCreateSemaphore(vk_context->logical_device, &semaphore_info, 0, &vk_context->submit_semaphore));


DEVICE_VULKAN_FUNCTION_BUILDER(vk_context->logical_device, vkCreateFence);
VkFenceCreateInfo fence_info = fence_create_info(VK_FENCE_CREATE_SIGNALED_BIT);
VK_CHECK(vkCreateFence(vk_context->logical_device, &fence_info, vk_context->allocator, &vk_context->image_available_fences));
//rustup docs --book

VkPipelineLayoutCreateInfo layout_info = {VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO};
layout_info.setLayoutCount = 1;
layout_info.pSetLayouts = 0; //&vk_context->set_layout;

DEVICE_VULKAN_FUNCTION_BUILDER(vk_context->logical_device, vkCreatePipelineLayout);

VK_CHECK(vkCreatePipelineLayout(vk_context->logical_device, &layout_info, vk_context->allocator, &vk_context->pipeline_layout));
/*
VkPipelineVertexInputStateCreateInfo vertex_input_state = {VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO};
VkPipelineColorBlendAttachmentState colour_blend_attachment = {};
colour_blend_attachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT  | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

colour_blend_attachment.blendEnable = VK_FALSE;
VkPipelineColorBlendStateCreateInfo colour_blend_state = {VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO};
colour_blend_state.pAttachments = &colour_blend_attachment;
colour_blend_state.attachmentCount = 1;

VkPipelineInputAssemblyStateCreateInfo input_assembly = {VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO};
input_assembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

VkViewport viewport = {};
viewport.maxDepth = 1.0;
VkRect2D scissor = {};

VkPipelineViewportStateCreateInfo viewport_state = {VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO};
viewport_state.pViewports = &viewport;
viewport_state.viewportCount = 1;
viewport_state.pScissors = &scissor;
viewport_state.scissorCount = 1;

VkPipelineRasterizationStateCreateInfo  rasterization_state = {VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO};
rasterization_state.cullMode = VK_CULL_MODE_BACK_BIT;
rasterization_state.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
rasterization_state.polygonMode = VK_POLYGON_MODE_FILL;
rasterization_state.lineWidth = 1.0f;

VkPipelineMultisampleStateCreateInfo multi_sample_state ={VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO};
multi_sample_state.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
*/
VkShaderModule vertexShader, fragmentShader;

  return true;
//notes: switch context->allocator, do macro madness for loading functions, = done. maybe mka e afucntion loader with cmjratori switch?
//and fix device selection.
//may need to double check stuff like vkimages[5]- are we oing imges? ie check up to video 21, not after
//double imageviews. double check everything
//do 600 lines one day, duble check the nexxt/ clean up codebase/make framework;
//maybe repalce kohi stuff?
//test thatvalidation layers are woorking- done
//use renderdoc

return true;

};

int main(){

   xcb_types x_context = {};

 vulkan_types vulkan_context = {};
init_xwindow(&x_context);
  if(!vk_init(&vulkan_context, &x_context)){

   // return 0;
  }
  

xwindow_main_loop(&x_context);
//GLOBAL_VULKAN_FUNCTION_BUILDER(vkGetInstanceProcAddr);
//INSTANCE_VULKAN_FUNCTION_BUILDER(vulkan_context.instance, vkDestroyInstance);
//vkDestroyInstance(vulkan_context.instance, vulkan_context.allocator);

}



