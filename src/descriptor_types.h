#pragma once

#include "push_data.h"
#include "petes_math.h"
#include "transfor_func.h"
#include "material_types.h"


enum asset_type_id{
    
    ASSET_SPRITE_WHITE,
    ASSET_SPRITE_DEFAULT,
    ASSET_COUNT,
};


struct Material{
    
    asset_type_id asset_id;
    Material_Data int_material_data;
    
};


struct Descriptor{
    
    VkDescriptorSet set;
    asset_type_id asset_id;
    
};



struct Render_Command{
    
    int instance_count;
    Descriptor * desc;
    Push_Data push_data;
};


static u32 render_command_count;
Render_Command render_commands[100];

PINLINE Render_Command * get_render_command(Descriptor * desc){
    
    Render_Command * rc = 0;
    if(render_command_count < 100){
        rc = &render_commands[render_command_count++];
        *rc = {};
        rc->desc = desc;
        rc->push_data.transform_index = transform_count;
        
    }else{
        
        printf("reached maximum amount of render commands");
    };
    return rc;
    
};