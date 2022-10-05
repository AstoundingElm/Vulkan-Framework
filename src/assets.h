#pragma once

#include "defines.h"
#include "file_types.h"
#include "vulkan_types.h"
#include "petes_math.h"
#include "entity.h"
#include "descriptor_types.h"
static tex_file_data default_data;

PINLINE const char * get_asset(asset_type_id asset_context){
    
    switch(asset_context){
        
        case ASSET_SPRITE_WHITE:{
            
            const char * default_texture  = "/home/petermiller/Desktop/4coder/hello/src/cover.jpg";
            return default_texture;
            break;
        }
        case ASSET_SPRITE_DEFAULT:{
            
            const char * white = "/home/petermiller/Desktop/4coder/hello/src/download.jpeg";
            return white;
            break;
        }
        case ASSET_COUNT:{
            
            break;
            
        }
    }
    
    const char * default_return = " huh?";
    return default_return;
    
};

u32 create_material(Game_State * game_state, asset_type_id asset_id, vec4 colour){
    
    u32 material_index = 0;
    
    if(game_state->material_count < MAX_MATERIALS){
        
        material_index = game_state->material_count;
        Material * m = &game_state->materials[game_state->material_count++];
        m->asset_id = asset_id;
        m->int_material_data.colour = colour;
        
    }else {
        
        
        
    }
    
    return material_index;
    
    
    
};


#define INVALID_INDEX UINT32_MAX
internal uint32_t get_material(Game_State *gameState, asset_type_id assetTypeID,
                               vec4 color)
{
    uint32_t materialIdx = INVALID_IDX;
    
    for (uint32_t i = 0; i < gameState->material_count; i++)
    {
        Material *m = &gameState->materials[i];
        
        if (m->asset_id == assetTypeID &&
            m->int_material_data.colour == color)
        {
            materialIdx = i;
            break;
        }
    }
    
    if (materialIdx == INVALID_IDX)
    {
        materialIdx = create_material(gameState, assetTypeID, color);
    }
    
    return materialIdx;
}

internal Material *get_material(Game_State *gameState, uint32_t materialIdx)
{
    /*CAKEZ_ASSERT(materialIdx < gameState->material_count, "MaterialIdx out of bounds");
    */
    Material *m = 0;
    
    if (materialIdx < gameState->material_count)
    {
        m = &gameState->materials[materialIdx];
    }
    else
    {
        // By default we return the first Material, this will default to ASSET_SPRITE_WHITE
        m = &gameState->materials[0];
    }
    
    return m;
}