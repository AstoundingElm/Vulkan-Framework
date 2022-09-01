#pragma once

#include "defines.h"
#include "vulkan_functions.h"
#include "vulkan_types.h"

VkOffset2D create_rect2D(VkOffset2D offset_2D){

    VkOffset2D offset = offset_2D;

    return offset;
};

VkExtent2D create_rect2D(VkExtent2D extent_2D){

VkExtent2D extent2D = extent_2D;

return extent2D;

};

VkRect2D create_rect2D(VkRect2D rect_2D){

VkRect2D rect2D = rect_2D;

return rect2D;

}