#pragma once

#include "defines.h"
#include "vulkan_functions.h"
#include "vulkan_types.h"

PINLINE VkPipelineDynamicStateCreateInfo create_dynamic_state_info(const void * pnext,VkPipelineDynamicStateCreateFlags flags, u32  dynamic_state_count, const VkDynamicState * dynamic_states){
    
    VkPipelineDynamicStateCreateInfo dynamic_info = {}; dynamic_info.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamic_info.pNext = pnext;
    dynamic_info.flags =  flags;
    dynamic_info.dynamicStateCount = dynamic_state_count; 
    dynamic_info.pDynamicStates = dynamic_states;
    
    return dynamic_info;
    
    
};



