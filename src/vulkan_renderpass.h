#pragma once

#include <typeinfo>
#include <tuple>
#include "defines.h"
#include "vulkan_functions.h"
#include "vulkan_types.h"


PINLINE VkRenderPassCreateInfo create_renderpass_info(const void *pointer_next, VkRenderPassCreateFlags renderpass_flags,
                                                      u32 attachment_count, const VkAttachmentDescription *p_attachments, u32 subpass_count,
                                                      const VkSubpassDescription *sub_passes, u32 dependency_count, const VkSubpassDependency *subpass_dependencies) {
    VkRenderPassCreateInfo renderpass_create_info = {};
    renderpass_create_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderpass_create_info.pNext = pointer_next;
    renderpass_create_info.flags = renderpass_flags;
    renderpass_create_info.attachmentCount = attachment_count;
    renderpass_create_info.pAttachments = p_attachments;
    renderpass_create_info.subpassCount = subpass_count;
    renderpass_create_info.pSubpasses = sub_passes;
    renderpass_create_info.dependencyCount = dependency_count;
    renderpass_create_info.pDependencies = subpass_dependencies;
    
    return renderpass_create_info;
    
};
PINLINE VkAttachmentDescription create_attachment_description(VkFormat format, VkSampleCountFlagBits sample_count_bits, VkAttachmentLoadOp
                                                              load_op, VkAttachmentStoreOp store_op,
                                                              VkImageLayout initial_layout,
                                                              VkImageLayout final_layout,
                                                              
                                                              
                                                              
                                                              VkAttachmentDescriptionFlags * attachment_flags = NULL,
                                                              VkAttachmentLoadOp * stencil_load_op = NULL,
                                                              VkAttachmentStoreOp * stencil_store_op = NULL) {
    VkAttachmentDescription colour_attachment;
    colour_attachment.flags = attachment_flags ? *attachment_flags: 0;
    colour_attachment.format = format;
    colour_attachment.samples = sample_count_bits;
    colour_attachment.loadOp = load_op;
    colour_attachment.storeOp = store_op;
    colour_attachment.stencilLoadOp = stencil_load_op ? *stencil_load_op : VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colour_attachment.stencilStoreOp = stencil_store_op ? * stencil_store_op : VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colour_attachment.initialLayout = initial_layout;
    colour_attachment.finalLayout =  final_layout;
    
    return colour_attachment;
};


PINLINE VkAttachmentReference create_attachment_reference(u32 attach, VkImageLayout attachment_reference_layout) {
    VkAttachmentReference attachment_reference;
    attachment_reference.attachment = attach;
    attachment_reference.layout = attachment_reference_layout;
    
    return attachment_reference;
};

PINLINE VkSubpassDescription create_subpass_desc(u32 colour_attachment_count, const VkAttachmentReference *colour_attachments,
                                                 VkSubpassDescriptionFlags * subpass_description_flags = NULL, VkPipelineBindPoint * pipeline_bind_point = NULL,
                                                 const VkAttachmentReference *input_attachments = NULL,
                                                 u32 * input_attachment_count = NULL,
                                                 const VkAttachmentReference *resolve_attachments = NULL,
                                                 const VkAttachmentReference *depth_stencil_attachment_reference = NULL, u32 * preserve_attachment_count = NULL, const u32 *preserve_attachments = NULL) {
    VkSubpassDescription subpass_desc = {};
    
    subpass_desc.flags = subpass_description_flags ? *subpass_description_flags: 0; 
    subpass_desc.pipelineBindPoint = pipeline_bind_point ? *pipeline_bind_point: VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass_desc.inputAttachmentCount =input_attachment_count ? * input_attachment_count : 0;
    subpass_desc.pInputAttachments =  input_attachments ? input_attachments : 0;
    subpass_desc.colorAttachmentCount = colour_attachment_count;
    subpass_desc.pColorAttachments = colour_attachments;
    subpass_desc.pResolveAttachments = resolve_attachments ? resolve_attachments : NULL;
    subpass_desc.pDepthStencilAttachment = depth_stencil_attachment_reference ?  depth_stencil_attachment_reference : NULL ;
    subpass_desc.preserveAttachmentCount = preserve_attachment_count ? * preserve_attachment_count :  0;
    subpass_desc.pPreserveAttachments = preserve_attachments ? preserve_attachments : NULL ;
    return subpass_desc;
};

PINLINE VkSubpassDependency create_subpass_dependency(u32 src_subpass, u32 dst_subpass, VkPipelineStageFlags src_stage_mask, 
                                                      VkPipelineStageFlags dst_stage_mask, VkAccessFlags src_access_mask, VkAccessFlags dst_access_mask, VkDependencyFlags dependency_flags){
    VkSubpassDependency subpass_dependency;
    subpass_dependency.srcSubpass = src_subpass;
    subpass_dependency.dstSubpass = dst_subpass;
    subpass_dependency.srcStageMask = src_stage_mask;
    subpass_dependency.dstStageMask = dst_stage_mask;
    subpass_dependency.srcAccessMask = src_access_mask;
    subpass_dependency.dstAccessMask = dst_access_mask;
    subpass_dependency.dependencyFlags = dependency_flags;
    return subpass_dependency;
};

PINLINE VkResult create_renderpass(surface_types * surface_context, logical_device_types * logical_context,renderpass_types * render_context ){
    
    VkAttachmentDescription colour_attachment = create_attachment_description(surface_context->surface_formats.format,
                                                                              VK_SAMPLE_COUNT_1_BIT,
                                                                              VK_ATTACHMENT_LOAD_OP_CLEAR,  VK_ATTACHMENT_STORE_OP_STORE, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);
    
    VkAttachmentDescription attachments[] = {colour_attachment};
    
    VkAttachmentReference colour_attachment_ref = create_attachment_reference( 0,VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL); 
    
    
    VkSubpassDescription subpass_desc = create_subpass_desc(1, &colour_attachment_ref);
    VkRenderPassCreateInfo renderpass_info = create_renderpass_info(NULL, 0, ArraySize(attachments), attachments, 1, &subpass_desc, 0, NULL);
    VK_CHECK(vkCreateRenderPass(logical_context->logical_device, &renderpass_info, 0, &render_context->renderpass));
    
    return VK_SUCCESS;
    
};