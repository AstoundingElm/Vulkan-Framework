#pragma once

#include "defines.h"
#include "vulkan_functions.h"
#include "vulkan_types.h"

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

  PINLINE char * linux_read_file(void * LinuxHandle, uint64_t Offset, uint64_t Size, void *Dest){

        //internal PLATFORM_READ_DATA_FROM_FILE(LinuxReadDataFromFile)
void *handle = nullptr;
    if(true)
    {
        int32_t LinuxHandle = *(int32_t *)handle;
        
        int64_t BytesRead = pread(LinuxHandle, Dest, Size, (int64_t)Offset);
        if ((int64_t)Size == BytesRead)
        {
            // NOTE(michiel): File read succeeded!
        }
        else
        {
            return 0;
            //LinuxFileError(Handle, "Read file failed.");
        }
    }
char * temp = 0;
    return temp;
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

global_variable shader_file_info file_info;
PINLINE void read_shader_file(){
//linux_read_file()
    file_info.vert_file = NULL;
    file_info.frag_file = NULL;
    file_info.vert_file = fopen("vert.spv", "rb+");

    file_info.frag_file = fopen("frag.spv", "rb+");
    file_info.shader_loaded = 1;
    if (file_info.vert_file == NULL || file_info.frag_file == NULL) {
       file_info.shader_loaded = 0;
        printf("could loadnt spv files");
    };
    fseek(file_info.vert_file, 0, SEEK_END);
    fseek(file_info.frag_file, 0, SEEK_END);

    file_info.vert_size = ftell(file_info.vert_file);
    file_info.frag_size = ftell(file_info.frag_file);

    file_info.vert_code = (char *)malloc(file_info.vert_size * sizeof(char));
    file_info.frag_code = (char *)malloc(file_info.frag_size * sizeof(char));

    rewind(file_info.vert_file);
    rewind(file_info.frag_file);
    fread(file_info.vert_code, 1, file_info.vert_size, file_info.vert_file);
    fread(file_info.frag_code, 1, file_info.frag_size, file_info.frag_file);

    fclose(file_info.vert_file);
    fclose(file_info.frag_file);
};

PINLINE VkShaderModuleCreateInfo create_shader_module_info(const void * shader_module_pnext, VkShaderModuleCreateFlags shader_module_flags, 
  size_t shader_code_size, const u32 * shader_module_pcode){

VkShaderModuleCreateInfo shader_module_info = {VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO};
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
 VkPipelineShaderStageCreateInfo pipeline_shader_stage_info = {VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO};
pipeline_shader_stage_info.pNext = pipeline_shader_stage_pnext;
pipeline_shader_stage_info.flags = pipeline_shader_stage_flags;
pipeline_shader_stage_info.stage = pipeline_shader_stage_flag_bits;
pipeline_shader_stage_info.module = pipeline_shader_stage_module;
pipeline_shader_stage_info.pName = pipeline_shader_stage_name;
pipeline_shader_stage_info.pSpecializationInfo = pipeline_shader_stage_specialization_info;

return pipeline_shader_stage_info;

 }