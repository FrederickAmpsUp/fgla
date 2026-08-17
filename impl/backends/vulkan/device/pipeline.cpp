#include "fgla/backends/vulkan/render_pipeline.hpp"
#include <fgla/backends/vulkan/util.hpp>
#include <fgla/backends/vulkan/device.hpp>
#include <fgla/backends/vulkan/shader_module.hpp>

namespace fgla::backends::vulkan {

Result<RenderPipeline> DeviceImpl::create_render_pipeline(const RenderPipeline::Descriptor &desc) {
  // vulkan, what the fuck.

  VkPipelineShaderStageCreateInfo stages[2] = {};
  size_t stage_count = 1;

  stages[0] = {
    .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
    .stage = VK_SHADER_STAGE_VERTEX_BIT,
    .module = dynamic_cast<ShaderModuleImpl *>(fgla::internal::ImplAccessor::get_impl(desc.vertex.module))->get_shader_module(),
    .pName = desc.vertex.entry_point
  };

  if (desc.fragment.has_value()) {
    stages[1] = {
      .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
      .stage = VK_SHADER_STAGE_FRAGMENT_BIT,
      .module = dynamic_cast<ShaderModuleImpl *>(fgla::internal::ImplAccessor::get_impl(desc.fragment->module))->get_shader_module(),
      .pName = desc.fragment->entry_point
    };
    stage_count = 2;
  }

  // TODO: fill in from the decsriptor
  VkPipelineLayoutCreateInfo pipeline_layout_info = {};
  pipeline_layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;

  VkPipelineLayout layout;
  VkResult res = vkCreatePipelineLayout(this->device, &pipeline_layout_info, nullptr, &layout);
  if (res != VK_SUCCESS) {
    return Error(1, "Failed to create vulkan pipeline layout");
  }

  VkPipelineVertexInputStateCreateInfo vertex_input_state = {};
  vertex_input_state.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
  
  VkPrimitiveTopology primitive_topology;

  switch (desc.primitive.topology) {
    #define CASE(name) \
    case RenderPipeline::PrimitiveState::Topology:: name : \
      primitive_topology = VK_PRIMITIVE_TOPOLOGY_##name;
    break;

    CASE(POINT_LIST)
    CASE(LINE_LIST)
    CASE(LINE_STRIP)
    CASE(TRIANGLE_LIST)
    CASE(TRIANGLE_STRIP)

    #undef CASE
  }

  VkPipelineInputAssemblyStateCreateInfo input_assembly_state = {};
  input_assembly_state.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
  input_assembly_state.topology = primitive_topology;
  input_assembly_state.primitiveRestartEnable = VK_FALSE;

  VkDynamicState dynamic_states[] = {
    VK_DYNAMIC_STATE_VIEWPORT,
    VK_DYNAMIC_STATE_SCISSOR
  };

  VkPipelineDynamicStateCreateInfo dynamic_state = {};
  dynamic_state.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
  dynamic_state.dynamicStateCount = sizeof(dynamic_states) / sizeof(*dynamic_states);
  dynamic_state.pDynamicStates = dynamic_states;

  VkPipelineViewportStateCreateInfo viewport_state = {};
  viewport_state.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
  viewport_state.viewportCount = 1;
  viewport_state.scissorCount = 1;

  VkPipelineRasterizationStateCreateInfo rasterization_state = {};
  rasterization_state.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
  rasterization_state.polygonMode = VK_POLYGON_MODE_FILL;
  rasterization_state.cullMode = VK_CULL_MODE_NONE;
  rasterization_state.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
  rasterization_state.lineWidth = 1.0f;

  VkPipelineMultisampleStateCreateInfo multisampling_state = {};
  multisampling_state.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
  multisampling_state.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

  std::vector<VkPipelineColorBlendAttachmentState> color_blend_attachments;
  color_blend_attachments.reserve(desc.color_attachments.size());
  std::vector<VkFormat> color_formats;
  color_formats.reserve(desc.color_attachments.size());
  
  for (const auto &attachment : desc.color_attachments) {
    // TODO: wire this in with Descriptor
    VkPipelineColorBlendAttachmentState color_blend_attachment = {};
    color_blend_attachment.blendEnable = VK_FALSE;
    color_blend_attachment.colorWriteMask =
      VK_COLOR_COMPONENT_R_BIT |
      VK_COLOR_COMPONENT_G_BIT |
      VK_COLOR_COMPONENT_B_BIT |
      VK_COLOR_COMPONENT_A_BIT;
    
    color_blend_attachments.push_back(color_blend_attachment);
    color_formats.push_back(vulkanize(attachment.format));
  }

  VkPipelineColorBlendStateCreateInfo color_blend_state = {};
  color_blend_state.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
  color_blend_state.attachmentCount = color_blend_attachments.size();
  color_blend_state.pAttachments = color_blend_attachments.data();

  VkPipelineRenderingCreateInfo rendering = {};
  rendering.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO;
  rendering.colorAttachmentCount = color_formats.size();
  rendering.pColorAttachmentFormats = color_formats.data();

  VkGraphicsPipelineCreateInfo pipeline_info = {};
  pipeline_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;

  pipeline_info.stageCount = stage_count;
  pipeline_info.pStages = stages;

  pipeline_info.pVertexInputState = &vertex_input_state;
  pipeline_info.pInputAssemblyState = &input_assembly_state;
  pipeline_info.pViewportState = &viewport_state;
  pipeline_info.pRasterizationState = &rasterization_state;
  pipeline_info.pMultisampleState = &multisampling_state;
  pipeline_info.pColorBlendState = &color_blend_state;
  pipeline_info.pDynamicState = &dynamic_state;
  pipeline_info.layout = layout;

  VkPipeline pipeline;
  res = vkCreateGraphicsPipelines(
      this->device, VK_NULL_HANDLE,
      1, &pipeline_info,
      nullptr, &pipeline);

  return RenderPipeline::from_raw(
  std::make_unique<RenderPipelineImpl>(
    layout, pipeline, this->device    
  ));
}

} // fgla::backends::vulkan
