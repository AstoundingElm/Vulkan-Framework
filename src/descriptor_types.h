#pragma once

#include "push_data.h"
#include "petes_math.h"
#include "transfor_func.h"
#include "material_types.h"
#include "texture_utils.h"


enum asset_type_id{
    
    ASSET_SPRITE_WHITE,
    ASSET_SPRITE_BALL,
    ASSET_SPRITE_PADDLE,
    ASSET_SPRITE_FONT_ATLAS,
    
    ASSET_COUNT,
};


struct Texture{
    
    PVec2 size;
    PVec2 subsize;
    
};

Texture  get_texture(asset_type_id assetTypeID){
    
    Texture texture = {};
    // Scaled by 3x
    switch (assetTypeID)
    {
        case ASSET_SPRITE_WHITE:
        {
            texture.size = {1.0f, 1.0f};
            texture.subsize = texture.size;
        }
        break;
        
        case ASSET_SPRITE_BALL:
        {
            texture.size = {48.0f, 48.0f};
            texture.subsize = texture.size;
        }
        break;
        
        case ASSET_SPRITE_PADDLE:
        {
            texture.size = {48.0f, 96.0f};
            texture.subsize = texture.size;
        }
        break;
        
        case ASSET_SPRITE_FONT_ATLAS:
        {
            texture.size = {240.0f, 240.0f};
            texture.subsize = {15.0f, 30.0f};
        }
        break;
        
        default:
        printf("gjgj");
        //CAKEZ_ASSERT(0, "Unrecognized Asset Type ID: %d", assetTypeID);
    }
    
    return texture;
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

PINLINE void vk_add_transform(u32 materialIdx, asset_type_id assetTypeID,
                              PVec2 pos, u32 animationIdx = 0, PVec2 size = {}){
    
    if (transform_count < MAX_TRANSFORMS)
    {
        Texture texture = get_texture(assetTypeID);
        //uint32_t materialIdx = //vk_get_or_create_material_idx( colour);
        
        uint32_t subsizeX = texture.subsize.x;
        uint32_t subsizeY = texture.subsize.y;
        uint32_t cols = texture.size.x / (float)subsizeX;
        uint32_t rows = texture.size.y / (float)subsizeY;
        float uvWidth = 1.0f / (float)cols;
        float uvHeight = 1.0f / (float)rows;
        
        if (size.x && size.y)
        {
            subsizeX = size.x;
            subsizeY = size.y;
        }
        
        Transform t = {};
        t.material_index = materialIdx;
        t.xPos = pos.x;
        t.yPos = pos.y;
        t.sizeX = subsizeX;
        t.sizeY = subsizeY;
        t.topV = float(animationIdx / cols) * uvHeight;
        t.bottomV = t.topV + uvHeight;
        t.leftU = float(animationIdx % cols) * uvWidth;
        t.rightU = t.leftU + uvWidth;
        
        transforms[transform_count++] = t;
    }
    else
    {
        //CAKEZ_ASSERT(0, "Reached maximum amount of transforms!");
    }
};

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