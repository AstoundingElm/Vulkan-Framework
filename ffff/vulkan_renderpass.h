#pragma once

#include "defines.h"
#include "vulkan_functions.h"
#include "vulkan_types.h"


PINLINE VkRenderPassCreateInfo create_renderpass_info(const void *pointer_next, VkRenderPassCreateFlags renderpass_flags,
                                              u32 attachment_count, const VkAttachmentDescription *p_attachments, u32 subpass_count,
                                              const VkSubpassDescription *sub_passes, u32 dependency_count, const VkSubpassDependency *subpass_dependencies) {
    VkRenderPassCreateInfo renderpass_create_info = {VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO};
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

PINLINE VkAttachmentDescription create_attachment_description(VkAttachmentDescriptionFlags attachment_flags, VkFormat attachment_format,
                                                      VkSampleCountFlagBits sample_count_bits, VkAttachmentLoadOp load_op, VkAttachmentStoreOp store_op, VkAttachmentLoadOp stencil_load_op,
                                                      VkAttachmentStoreOp stencil_store_op, VkImageLayout initial_layout, VkImageLayout final_layout) {
    VkAttachmentDescription colour_attachment;
    colour_attachment.flags = attachment_flags;
    colour_attachment.format = attachment_format;
    colour_attachment.samples = sample_count_bits;
    colour_attachment.loadOp = load_op;
    colour_attachment.storeOp = store_op;
    colour_attachment.stencilLoadOp = stencil_load_op;
    colour_attachment.stencilStoreOp = stencil_store_op;
    colour_attachment.initialLayout = initial_layout;
    colour_attachment.finalLayout = final_layout;

    return colour_attachment;
};

PINLINE VkAttachmentReference create_attachment_reference(u32 attach, VkImageLayout attachment_reference_layout) {
    VkAttachmentReference attachment_reference = {};
    attachment_reference.attachment = attach;
    attachment_reference.layout = attachment_reference_layout;

    return attachment_reference;
};

PINLINE VkSubpassDescription create_subpass_desc(VkSubpassDescriptionFlags subpass_description_flags, VkPipelineBindPoint pipeline_bind_point,
                                         u32 input_attachment_count, const VkAttachmentReference *input_attachments,
                                         u32 colour_attachment_count, const VkAttachmentReference *colour_attachments, const VkAttachmentReference *resolve_attachments,
                                         const VkAttachmentReference *depth_stencil_attachment_reference, u32 preserve_attachment_count, const u32 *preserve_attachments) {
    VkSubpassDescription subpass_desc = {};
    subpass_desc.flags = subpass_description_flags;
    subpass_desc.pipelineBindPoint = pipeline_bind_point;
    subpass_desc.inputAttachmentCount = input_attachment_count;
    subpass_desc.pInputAttachments = input_attachments;
    subpass_desc.colorAttachmentCount = colour_attachment_count;
    subpass_desc.pColorAttachments = colour_attachments;
    subpass_desc.pResolveAttachments = resolve_attachments;
    subpass_desc.pDepthStencilAttachment = depth_stencil_attachment_reference;
    subpass_desc.preserveAttachmentCount = preserve_attachment_count;
    subpass_desc.pPreserveAttachments = preserve_attachments;
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