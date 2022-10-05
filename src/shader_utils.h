#pragma once

#include "defines.h"
#include "vulkan_functions.h"
#include "vulkan_types.h"
#include "file_types.h"
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

PINLINE bool read_shader_file(){
    shader_data.vert_file = NULL;
    shader_data.frag_file = NULL;
    shader_data.vert_file = fopen("/home/petermiller/Desktop/4coder/hello/src/vert.spv", "rb+");
    if(!shader_data.vert_file){
        printf("fail");
    }
    shader_data.frag_file = fopen("/home/petermiller/Desktop/4coder/hello/src/frag.spv", "rb+");
    shader_data.shader_loaded = 1;
    if (shader_data.vert_file == NULL || shader_data.frag_file == NULL) {
        shader_data.shader_loaded = 0;
        printf("could loadnt spv files");
        return false;
    };
    fseek(shader_data.vert_file, 0, SEEK_END);
    fseek(shader_data.frag_file, 0, SEEK_END);
    
    shader_data.vert_size = ftell(shader_data.vert_file);
    shader_data.frag_size = ftell(shader_data.frag_file);
    
    shader_data.vert_code = (char *)malloc(shader_data.vert_size * sizeof(char));
    shader_data.frag_code = (char *)malloc(shader_data.frag_size * sizeof(char));
    
    rewind(shader_data.vert_file);
    rewind(shader_data.frag_file);
    fread(shader_data.vert_code, 1, shader_data.vert_size, shader_data.vert_file);
    fread(shader_data.frag_code, 1, shader_data.frag_size, shader_data.frag_file);
    
    fclose(shader_data.vert_file);
    fclose(shader_data.frag_file);
    return true;
};

PINLINE VkShaderModuleCreateInfo create_shader_module_info(const void * shader_module_pnext, VkShaderModuleCreateFlags shader_module_flags, 
                                                           size_t shader_code_size, const u32 * shader_module_pcode){
    
    VkShaderModuleCreateInfo shader_module_info = {};
    shader_module_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    shader_module_info.pNext = shader_module_pnext;
    shader_module_info.flags = shader_module_flags;
    shader_module_info.codeSize = shader_code_size;
    shader_module_info.pCode = shader_module_pcode;
    
    return shader_module_info;
    
};

PINLINE VkPipelineShaderStageCreateInfo create_pipeline_shader_stage_info(const void * pipeline_shader_stage_pnext, 
                                                                          VkPipelineShaderStageCreateFlags pipeline_shader_stage_flags, VkShaderStageFlagBits pipeline_shader_stage_flag_bits,
                                                                          VkShaderModule pipeline_shader_stage_module, const char * pipeline_shader_stage_name, 
                                                                          const VkSpecializationInfo * pipeline_shader_stage_specialization_info){
    VkPipelineShaderStageCreateInfo pipeline_shader_stage_info = {};
    pipeline_shader_stage_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    pipeline_shader_stage_info.pNext = pipeline_shader_stage_pnext;
    pipeline_shader_stage_info.flags = pipeline_shader_stage_flags;
    pipeline_shader_stage_info.stage = pipeline_shader_stage_flag_bits;
    pipeline_shader_stage_info.module = pipeline_shader_stage_module;
    pipeline_shader_stage_info.pName = pipeline_shader_stage_name;
    pipeline_shader_stage_info.pSpecializationInfo = pipeline_shader_stage_specialization_info;
    
    return pipeline_shader_stage_info;
    
}