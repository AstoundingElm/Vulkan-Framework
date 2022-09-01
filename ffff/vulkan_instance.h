#pragma once
#include "vulkan_types.h"
#include "vulkan_functions.h"
#include "stdio.h"

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

PINLINE VkInstanceCreateInfo create_instance_info( const void * instance_pnext, VkInstanceCreateFlags instance_flags, VkApplicationInfo *application_info, u32 enabled_layer_count, const char *const *enabled_layer_names,
                                          u32 enabled_extension_count,
                                          const char *const *enabled_extension_names) {
    VkInstanceCreateInfo instance_create_info = {VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO};
    instance_create_info.pNext = instance_pnext;
    instance_create_info.flags = instance_flags;
    instance_create_info.pApplicationInfo = application_info;
    instance_create_info.enabledLayerCount = enabled_layer_count;
    instance_create_info.ppEnabledLayerNames = enabled_layer_names;
    instance_create_info.enabledExtensionCount = enabled_extension_count;
    instance_create_info.ppEnabledExtensionNames = enabled_extension_names;

    return instance_create_info;
};


PINLINE VkDebugUtilsMessengerCreateInfoEXT create_debug_info(VkDebugUtilsMessageSeverityFlagsEXT message_severity, VkDebugUtilsMessageTypeFlagsEXT message_type,
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

PINLINE VkApplicationInfo create_app_info(const void * application_pnext, const char *app_name, u32 app_version, const char *engine_name, u32 engine_version, u32 api_version) {
    VkApplicationInfo app_info = {VK_STRUCTURE_TYPE_APPLICATION_INFO};
    app_info.pNext = application_pnext;
    app_info.pApplicationName = app_name;
    app_info.applicationVersion = app_version;
    app_info.pEngineName = engine_name;
    app_info.engineVersion = engine_version;
    app_info.apiVersion = api_version;
    // 0, app_name, app_version, engine_name, engine_version, api_version};
    return app_info;
};

PINLINE VkResult create_instance(vulkan_types * vulkan_context){

    load_global_level_functions(&vk_context);
    vulkan_context->allocator = 0;
    VkApplicationInfo app_info = create_app_info(0, "Learn", VK_MAKE_VERSION(1, 0, 0), "test", VK_MAKE_VERSION(1, 0, 0), VK_API_VERSION_1_0);

    const char *extensions[] = {VK_KHR_SURFACE_EXTENSION_NAME, VK_KHR_XCB_SURFACE_EXTENSION_NAME, VK_EXT_DEBUG_UTILS_EXTENSION_NAME};
    const char *layers[] = {"VK_LAYER_KHRONOS_validation"};

    VkInstanceCreateInfo instance_info = create_instance_info(NULL, 0,&app_info, ArraySize(layers), layers, ArraySize(extensions), extensions);

    VK_CHECK(vkCreateInstance(&instance_info, vulkan_context->allocator, &vulkan_context->instance));

    load_instance_level_funcs(&vk_context);

       VkDebugUtilsMessengerCreateInfoEXT debug_info = create_debug_info(VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT, 
    VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT, vk_debug_callback);

VK_CHECK(vkCreateDebugUtilsMessengerEXT(vulkan_context->instance, &debug_info, vulkan_context->allocator, &vulkan_context->debug_messenger ));

return VK_SUCCESS;

}