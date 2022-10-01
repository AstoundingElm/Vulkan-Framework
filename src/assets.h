#pragma once

#include "defines.h"
#include "file_types.h"

enum asset_type_id{
    
    ASSET_SPRITE_WHITE,
    ASSET_SPRITE_DEFAULT,
    ASSET_COUNT,
};

static tex_file_data default_data;

PINLINE const char * get_asset(asset_type_id asset_context){
    
    switch(asset_context){
        
        case ASSET_SPRITE_WHITE:{
            char * white = new char[4];
            white[0] = (char)255;
            white[1] = (char)255;
            white[2] = (char)255;
            white[3] = (char)255;
            return (const char *)white;
            break;
        }
        case ASSET_SPRITE_DEFAULT:{
            default_data.pixels  = stbi_load("/home/petermiller/Desktop/4coder/hello/src/download.jpeg", &default_data.tex_width,& default_data.tex_height, &default_data.tex_channels, STBI_rgb_alpha);
            
            
            VkDeviceSize image_size;
            image_size = load_texture_data.tex_width * load_texture_data.tex_height * 4;
            const char * white = "/home/petermiller/Desktop/4coder/hello/src/download.jpeg";
            return white;
            break;
        }
        case ASSET_COUNT:{
            
            break;
            
        }
    }
    const char * white = 0;
    //temporary
    return white;
    
};
