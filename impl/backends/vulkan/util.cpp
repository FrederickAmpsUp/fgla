#include <fgla/backends/vulkan/util.hpp>

namespace fgla::backends::vulkan {

void signal_timeline_from_binary(VkDevice device, VkQueue queue,
                                 VkSemaphore binary_semaphore,
                                 VkSemaphore timeline_semaphore,
                                 uint64_t timeline_value) {
  VkTimelineSemaphoreSubmitInfo timeline_info = {};
  timeline_info.sType = VK_STRUCTURE_TYPE_TIMELINE_SEMAPHORE_SUBMIT_INFO;
  timeline_info.waitSemaphoreValueCount = 0;
  timeline_info.pWaitSemaphoreValues = nullptr;
  timeline_info.signalSemaphoreValueCount = 1;
  timeline_info.pSignalSemaphoreValues = &timeline_value;

  VkPipelineStageFlags stage_mask =
    VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;

  VkSubmitInfo submit{};
  submit.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
  submit.pNext = &timeline_info;
  submit.waitSemaphoreCount = 1;
  submit.pWaitSemaphores = &binary_semaphore;
  submit.pWaitDstStageMask = &stage_mask;
  submit.signalSemaphoreCount = 1;
  submit.pSignalSemaphores = &timeline_semaphore;
  submit.commandBufferCount = 0;

  vkQueueSubmit(queue, 1, &submit, VK_NULL_HANDLE);
}

void signal_binary_from_timelines(
    VkDevice device, VkQueue queue,
    const std::vector<VkSemaphore> &timeline_semaphores,
    const std::vector<uint64_t> &semaphore_values,
    VkSemaphore binary_semaphore) {
  VkTimelineSemaphoreSubmitInfo timeline_info = {};
  timeline_info.sType = VK_STRUCTURE_TYPE_TIMELINE_SEMAPHORE_SUBMIT_INFO;

  timeline_info.waitSemaphoreValueCount = semaphore_values.size();
  timeline_info.pWaitSemaphoreValues = semaphore_values.data();
  timeline_info.signalSemaphoreValueCount = 0;
  timeline_info.pSignalSemaphoreValues = nullptr;

  VkSubmitInfo submit{};
  submit.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
  submit.pNext = &timeline_info;
  submit.waitSemaphoreCount = timeline_semaphores.size();
  submit.pWaitSemaphores = timeline_semaphores.data();
  submit.signalSemaphoreCount = 1;
  submit.pSignalSemaphores = &binary_semaphore;
  submit.commandBufferCount = 0;

  vkQueueSubmit(queue, 1, &submit, VK_NULL_HANDLE);
}
} // namespace fgla::backends::vulkan
