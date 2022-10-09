#pragma once

#include "defines.h"
#include "vulkan_functions.h"
#include "vulkan_types.h"


struct tex_file_data{
    int tex_width;
    int tex_height;
    int tex_channels;
    stbi_uc * pixels;
    
};


struct shader_file_info{
    
    FILE * vert_file;
    FILE * frag_file;
    u32 vert_size;
    u32 frag_size;
    char * vert_code;
    char * frag_code;
    char shader_loaded;
    
};

global_variable shader_file_info shader_data = {};
global_variable tex_file_data load_image_data = {};
global_variable tex_file_data load_texture_data;
