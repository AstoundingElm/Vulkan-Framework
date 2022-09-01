#pragma once

#include "defines.h"
#include "vulkan_functions.h"
#include "vulkan_types.h"

struct pipeline_types{


};

PINLINE VkGraphicsPipelineCreateInfo  create_graphics_pipeline_info(u32 stage_count, const VkPipelineShaderStageCreateInfo * p_stages, 
const VkPipelineVertexInputStateCreateInfo* pvertex_input_state, const VkPipelineInputAssemblyStateCreateInfo* pinput_assembly_state,
 const VkPipelineTessellationStateCreateInfo* ptessellation_state,   const VkPipelineViewportStateCreateInfo*  pviewport_state, 
 const VkPipelineRasterizationStateCreateInfo*  prasterization_state,  const VkPipelineMultisampleStateCreateInfo* pmultisample_state, 
   const VkPipelineDepthStencilStateCreateInfo* pdepth_stencil_state, const VkPipelineColorBlendStateCreateInfo*   pcolour_blend_state, 
  const VkPipelineDynamicStateCreateInfo*   pdynamic_state, VkPipelineLayout pipe_layout, VkRenderPass pipeline_renderpass, 
  u32 pipeline_subpass, VkPipeline base_pipeline_handle, int32_t base_pipeline_index

 ){

    VkGraphicsPipelineCreateInfo graphics_pipeline_create_info = {VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO};
    graphics_pipeline_create_info.pNext = NULL;
    graphics_pipeline_create_info.flags = 0;// VK_PIPELINE_CREATE_DERIVATIVE_BIT;
    graphics_pipeline_create_info.stageCount = stage_count;
    graphics_pipeline_create_info.pStages = p_stages;
    graphics_pipeline_create_info.pVertexInputState = pvertex_input_state; 
    graphics_pipeline_create_info.pInputAssemblyState = pinput_assembly_state;
      graphics_pipeline_create_info.pTessellationState = ptessellation_state;
      graphics_pipeline_create_info.pViewportState = pviewport_state;
     graphics_pipeline_create_info. pRasterizationState = prasterization_state;
        graphics_pipeline_create_info. pMultisampleState = pmultisample_state;
     graphics_pipeline_create_info.pDepthStencilState = pdepth_stencil_state;
        graphics_pipeline_create_info.pColorBlendState = pcolour_blend_state;
         graphics_pipeline_create_info.pDynamicState = pdynamic_state;
   graphics_pipeline_create_info.layout = pipe_layout;
      graphics_pipeline_create_info.renderPass = pipeline_renderpass;
  graphics_pipeline_create_info.subpass = pipeline_subpass;
    graphics_pipeline_create_info.basePipelineHandle = base_pipeline_handle;
  
    graphics_pipeline_create_info.basePipelineIndex = base_pipeline_index;

    return graphics_pipeline_create_info;
};

PINLINE VkPipelineVertexInputStateCreateInfo create_vertex_input_state(u32 vertex_binding_description_count, const VkVertexInputBindingDescription *
pvertex_binding_descriptions, u32 vertex_attribute_description_count, const VkVertexInputAttributeDescription * pvertex_attribute_descriptions ) {
    VkPipelineVertexInputStateCreateInfo vertex_input_state = {VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO};
    
    vertex_input_state.pNext = NULL;
    vertex_input_state.flags = 0;
    vertex_input_state.vertexBindingDescriptionCount = vertex_binding_description_count;
    vertex_input_state.pVertexBindingDescriptions = pvertex_binding_descriptions;
    vertex_input_state.vertexAttributeDescriptionCount = vertex_attribute_description_count;
    vertex_input_state.pVertexAttributeDescriptions = pvertex_attribute_descriptions;
    
    return vertex_input_state;
};

PINLINE VkPipelineColorBlendAttachmentState create_color_blend_attach_state(VkBool32 blend_enable,  VkBlendFactor src_colour_blend_factor,
VkBlendFactor dst_colour_blend_factor, VkBlendOp colour_blend_op, VkBlendFactor src_alpha_blend_factor, 
VkBlendFactor dst_alpha_blend_factor, VkBlendOp alpha_blend_op,  VkColorComponentFlags colour_write_mask) {
    VkPipelineColorBlendAttachmentState colour_blend_attachment = {};
    colour_blend_attachment.blendEnable = blend_enable;
    colour_blend_attachment.srcColorBlendFactor = src_colour_blend_factor;
    colour_blend_attachment.dstColorBlendFactor = dst_colour_blend_factor;
    colour_blend_attachment.colorBlendOp = colour_blend_op;
    colour_blend_attachment.srcAlphaBlendFactor = src_alpha_blend_factor;
    colour_blend_attachment.dstAlphaBlendFactor = dst_alpha_blend_factor;
    colour_blend_attachment.alphaBlendOp = alpha_blend_op;
    colour_blend_attachment.colorWriteMask = colour_write_mask;

    return colour_blend_attachment;
}

PINLINE VkPipelineColorBlendStateCreateInfo create_colour_blend_state(VkBool32 logic_op_enable, VkLogicOp logic_op, 
u32 attachment_count, const VkPipelineColorBlendAttachmentState *p_attachments) {
    VkPipelineColorBlendStateCreateInfo colour_blend_state = {VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO};
     colour_blend_state.pNext = NULL;
     colour_blend_state.flags = 0;
     colour_blend_state.logicOpEnable = logic_op_enable;
     colour_blend_state.logicOp = logic_op;
    colour_blend_state.attachmentCount = attachment_count;
    colour_blend_state.pAttachments = p_attachments;
    for(u32 i=0;i<4;i++){
		colour_blend_state.blendConstants[i]=0.0f;
    }
    return colour_blend_state;
}

PINLINE VkPipelineInputAssemblyStateCreateInfo create_input_assembly(VkPrimitiveTopology input_assembly_topology) {
    VkPipelineInputAssemblyStateCreateInfo input_assembly = {VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO};
     input_assembly.pNext = NULL;
     input_assembly.flags = 0;
    input_assembly.topology = input_assembly_topology;
     input_assembly.primitiveRestartEnable = VK_FALSE;

    return input_assembly;
}

PINLINE VkPipelineViewportStateCreateInfo create_viewport_state_info(u32 viewport_count, const VkViewport *view_port, u32 scissor_count, const VkRect2D *scissor) {
    VkPipelineViewportStateCreateInfo viewport_state = {VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO};
     viewport_state.pNext = NULL;
     viewport_state.flags = 0;
    viewport_state.viewportCount = viewport_count;
    viewport_state.pViewports = view_port;
    viewport_state.scissorCount = scissor_count;
    viewport_state.pScissors = scissor;

    return viewport_state;
}

PINLINE VkPipelineRasterizationStateCreateInfo create_rasterization_state_info(VkBool32 depth_clamp_enable, VkBool32 rasterizer_discard_enable,
    VkPolygonMode polygon_mode, VkCullModeFlags cull_mode, VkFrontFace front_face, VkBool32 depth_bias_enable, float depth_bias_constant_factor,
    float depth_bias_clamp, float depth_bias_slope_factor, float line_width) {
    VkPipelineRasterizationStateCreateInfo rasterization_state = {VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO};
    rasterization_state.pNext = NULL;
    rasterization_state.flags = 0;
    rasterization_state.depthClampEnable = depth_clamp_enable;
    rasterization_state.rasterizerDiscardEnable = rasterizer_discard_enable;
    rasterization_state.polygonMode = polygon_mode;
    rasterization_state.cullMode = cull_mode;
    rasterization_state.frontFace = front_face;
    rasterization_state.depthBiasEnable = depth_bias_enable;
    rasterization_state.depthBiasConstantFactor = depth_bias_constant_factor;
    rasterization_state.depthBiasClamp = depth_bias_clamp;
    rasterization_state.depthBiasSlopeFactor = depth_bias_slope_factor;
    rasterization_state.lineWidth = line_width;

    return rasterization_state;
}

PINLINE VkPipelineMultisampleStateCreateInfo create_multisample_state_info(VkSampleCountFlagBits rasterization_samples, VkBool32 sample_shading_enable, 
float min_sample_shading, const VkSampleMask * sample_mask, VkBool32 alpha_to_coverage_enable, VkBool32 alpha_to_one_enable) {
    VkPipelineMultisampleStateCreateInfo multi_sample_state = {VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO};
     multi_sample_state.pNext = NULL;
     multi_sample_state.flags = 0;
    multi_sample_state.rasterizationSamples = rasterization_samples;
     multi_sample_state.sampleShadingEnable = sample_shading_enable;
     multi_sample_state.minSampleShading = min_sample_shading;
     multi_sample_state.pSampleMask = sample_mask;
     multi_sample_state.alphaToCoverageEnable = alpha_to_coverage_enable;
     multi_sample_state.alphaToOneEnable = alpha_to_one_enable;

    return multi_sample_state;
};

PINLINE VkPipelineLayoutCreateInfo create_pipeline_layout_info(u32 set_layout_count, const VkDescriptorSetLayout * pset_layouts, u32 push_constant_range_count,
const VkPushConstantRange * ppush_constant_ranges){

VkPipelineLayoutCreateInfo pipeline_layout_create_info = {VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO};
pipeline_layout_create_info.pNext = NULL;
pipeline_layout_create_info.flags = 0;
pipeline_layout_create_info.setLayoutCount = set_layout_count;
pipeline_layout_create_info.pSetLayouts = pset_layouts;
pipeline_layout_create_info.pushConstantRangeCount = push_constant_range_count;
pipeline_layout_create_info.pPushConstantRanges = ppush_constant_ranges;

return pipeline_layout_create_info;

}