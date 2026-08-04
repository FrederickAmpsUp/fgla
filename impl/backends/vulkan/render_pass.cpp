#include <fgla/backends/vulkan/render_pass.hpp>

namespace fgla::backends::vulkan {

RenderPassImpl::~RenderPassImpl() { vkCmdEndRendering(this->command_buffer); }

} // namespace fgla::backends::vulkan
