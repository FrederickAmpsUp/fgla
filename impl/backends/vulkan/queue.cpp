#include <fgla/backends/vulkan/adapter.hpp>
#include <fgla/backends/vulkan/command_buffer.hpp>
#include <fgla/backends/vulkan/device.hpp>
#include <fgla/backends/vulkan/queue.hpp>
#include <vulkan/vulkan_core.h>
#ifdef FGLA_VK_EXT_WINDOWING
#include <fgla/backends/vulkan/ext/windowing/surface.hpp>
#endif
#include <fgla/backends/vulkan/completion.hpp>
#include <spdlog/spdlog.h>

namespace std {
template <> struct hash<fgla::Queue::Request> {
  std::size_t operator()(const fgla::Queue::Request &req) const noexcept {
    return (std::hash<int>()(static_cast<int>(req.type)) << 1) ^
           std::hash<uint32_t>()(req.count);
  }
};
} // namespace std

namespace fgla::backends::vulkan {

bool operator==(const Queue::Request &lhs, const Queue::Request &rhs) {
  return lhs.type == rhs.type && lhs.count == rhs.count;
}

std::pair<std::vector<VkDeviceQueueCreateInfo>, QueueAllocator::QueueMapping>
QueueAllocator::big_brain_allocator_algorithm(
    const std::initializer_list<Queue::Request> &requests,
    std::vector<VkQueueFamilyProperties> families) {
  static auto logger = spdlog::get("fgla::backends::vulkan");

  struct FamilyInfo {
    uint32_t index;
    VkQueueFlags flags;
    uint32_t max_queues;
    std::vector<Queue::Type> used_queues = {};
  };

  std::vector<FamilyInfo> family_infos;

  uint32_t i = 0;
  for (auto family : families) {
    if (family.queueFlags != 0) {
      family_infos.push_back({.index = i,
                              .flags = family.queueFlags,
                              .max_queues = family.queueCount});
    }

    ++i;
  }

  std::unordered_map<Queue::Type,
                     std::pair<uint32_t, std::vector<FamilyInfo *>>>
      counts_and_available_families;

  for (auto request : requests) {
    std::vector<FamilyInfo *> supported_families;
    for (auto &family : family_infos) {
      bool supported;
      switch (request.type) {
      case Queue::Type::Graphics:
        supported = family.flags & VK_QUEUE_GRAPHICS_BIT;
        break;
      case Queue::Type::Transfer:
        supported = family.flags & VK_QUEUE_TRANSFER_BIT;
        break;
#ifdef FGLA_VK_EXT_WINDOWING
      case fgla::ext::windowing::QueueTypeExt::Present: {
        auto &present_queue_opts =
            *reinterpret_cast<fgla::ext::windowing::PresentQueueOptions *>(
                request.user_data);
        VkSurfaceKHR vk_surface =
            present_queue_opts.surface.to_impl<ext::windowing::SurfaceImpl>()
                .get_surface();
        VkPhysicalDevice vk_device =
            present_queue_opts.adapter.to_impl<AdapterImpl>()
                .get_physical_device();

        VkBool32 present_support = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(vk_device, family.index,
                                             vk_surface, &present_support);

        supported = present_support;
      } break;
#endif
      default:
        logger->warn("Unknown logical queue type!");
        supported = false;
        break;
      }
      if (supported && family.max_queues > 0)
        supported_families.push_back(&family);
    }
    if (counts_and_available_families.find(request.type) !=
        counts_and_available_families.end()) {
      counts_and_available_families[request.type].first += request.count;
    } else {
      counts_and_available_families.insert(
          {request.type, {request.count, supported_families}});
    }
  }

  QueueMapping final_mapping;

  for (auto [type, caf] : counts_and_available_families) {
    auto [count, supported_families] = caf;
    for (int i = 0; i < count; ++i) {
      FamilyInfo *best_family = nullptr;
      int best_score = -3;
      int idx_fallback_queue = 0;
      for (FamilyInfo *family : supported_families) {
        int score = 0;

        if (family->used_queues.empty()) score += 2;

        if (family->used_queues.size() == family->max_queues) score = -2;

        int has_same = 0;
        int j = 0;
        for (auto used_type : family->used_queues) {
          if (used_type == type) {
            has_same = 1;
            idx_fallback_queue = j;
          }
          ++j;
        }
        score += has_same;

        if (score >= best_score) {
          best_family = family;
          best_score = score;
        }
      }
      if (best_score >= 0 && best_family) {
        final_mapping.insert(
            {{type, i}, {best_family->index, best_family->used_queues.size()}});
        best_family->used_queues.push_back(type);
      } else if (best_family) {
        final_mapping.insert(
            {{type, i}, {best_family->index, idx_fallback_queue}});
      } else {
        logger->error("Unsupported requested queue, ignoring!");
      }
    }
  }

  std::vector<VkDeviceQueueCreateInfo> queue_create_infos;

  for (const auto &family : family_infos) {
    if (family.used_queues.empty()) continue;
    VkDeviceQueueCreateInfo queue_create_info = {};
    queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;

    queue_create_info.queueFamilyIndex = family.index;
    queue_create_info.queueCount = family.used_queues.size();

    static float PRIORITIES[16] = {1.0f};

    queue_create_info.pQueuePriorities = PRIORITIES;

    queue_create_infos.push_back(queue_create_info);
  }

  return std::make_pair(queue_create_infos, final_mapping);
}

QueueAllocator::QueueAllocator(
    const std::initializer_list<Queue::Request> &requests,
    VkPhysicalDevice physical_device) {
  uint32_t n_queue_families = 0;
  vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &n_queue_families,
                                           nullptr);

  std::vector<VkQueueFamilyProperties> queue_families(n_queue_families);
  vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &n_queue_families,
                                           queue_families.data());

  auto result = big_brain_allocator_algorithm(requests, queue_families);

  this->create_infos = result.first;
  this->queue_mapping = result.second;
}

QueueAllocator::Queues QueueAllocator::get_queues(VkDevice device) {
  Queues queues;

  for (auto [queue_handle, queue] : this->queue_mapping) {
    VkQueue vk_queue;
    vkGetDeviceQueue(device, queue.first, queue.second, &vk_queue);

    std::unique_ptr<QueueImpl> queue_impl =
        std::make_unique<QueueImpl>(vk_queue, queue.first);
    Queue fg_queue = Queue::from_impl(std::move(queue_impl));

    queues.insert({queue_handle, std::move(fg_queue)});
  }

  return queues;
}

void QueueImpl::init(VkDevice device, std::vector<VkSemaphore> *semaphore_pool,
                     VkCommandPool command_pool) {
  static auto logger = spdlog::get("fgla::backends::vulkan");
  this->device = device;
  this->semaphore_pool = semaphore_pool;
  this->command_pool = command_pool;

  VkSemaphoreCreateInfo semaphore_info = {};
  semaphore_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

  VkSemaphoreTypeCreateInfo semaphore_type_info = {};
  semaphore_type_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_TYPE_CREATE_INFO;
  semaphore_type_info.semaphoreType = VK_SEMAPHORE_TYPE_TIMELINE;
  semaphore_type_info.initialValue = this->timeline_value;

  semaphore_info.pNext = &semaphore_type_info;

  if (vkCreateSemaphore(this->device, &semaphore_info, nullptr,
                        &this->timeline) != VK_SUCCESS) {
    logger->error("Failed to initialize queue!");
  }

  logger->info("Initialized queue.");
}

Result<CommandBuffer> QueueImpl::init_cb(VkCommandBuffer command_buffer,
                                         VkFence fence) {
  vkResetCommandBuffer(command_buffer, 0);
  vkResetFences(this->device, 1, &fence);

  VkCommandBufferBeginInfo begin_info = {};
  begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

  if (vkBeginCommandBuffer(command_buffer, &begin_info) != VK_SUCCESS) {
    return Error(2, "Failed to begin Vulkan command buffer");
  }

  return CommandBuffer::from_impl(
      std::make_unique<CommandBufferImpl>(command_buffer, fence));
}

Result<CommandBuffer> QueueImpl::begin_recording() {
  static auto logger = spdlog::get("fgla::backends::vulkan");

  for (auto &[command_buffer, fence] : this->command_buffer_pool) {
    VkResult state = vkGetFenceStatus(this->device, fence);
    if (state == VK_SUCCESS) { // the command buffer has finished and we can
                               // reset and use it
      return this->init_cb(command_buffer, fence);
    }
  }

  if (this->command_pool == VK_NULL_HANDLE) {
    return Error(
        1, "Failed to allocate Vulkan command buffer - no command pool found");
  }

  VkCommandBufferAllocateInfo alloc_info = {};
  alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  alloc_info.commandPool = this->command_pool;
  alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  alloc_info.commandBufferCount = 1;

  VkCommandBuffer command_buffer;
  VkResult res =
      vkAllocateCommandBuffers(this->device, &alloc_info, &command_buffer);
  if (res != VK_SUCCESS) {
    return Error(-1, "Failed to allocate Vulkan command buffer");
  }

  logger->info("Allocated Vulkan command buffer.");

  VkFenceCreateInfo create_info = {};
  create_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
  create_info.pNext = nullptr;
  create_info.flags = 0;

  VkFence fence;
  res = vkCreateFence(this->device, &create_info, nullptr, &fence);
  if (res != VK_SUCCESS) {
    // destroy command buffer please
    return Error(-2, "Failed to create Vulkan fence");
  }

  logger->info("Created Vulkan fence.");

  this->command_buffer_pool.insert({command_buffer, fence});

  return this->init_cb(command_buffer, fence);
}

Result<Completion>
QueueImpl::submit(CommandBuffer &&cb,
                  std::initializer_list<fgla::Completion> wait_completions) {
  static auto logger = spdlog::get("fgla::backends::vulkan");

  CommandBufferImpl &command_buffer = cb.to_impl<CommandBufferImpl>();

  command_buffer.end_recording();

  VkSubmitInfo2 submit_info = {};
  submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO_2;

  std::vector<VkSemaphoreSubmitInfo> wait_semaphores;
  wait_semaphores.reserve(wait_completions.size());

  for (const auto &completion : wait_completions) {
    auto &completion_impl = completion.to_impl<CompletionImpl>();

    VkSemaphoreSubmitInfo wait_semaphore = {};
    wait_semaphore.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO;
    wait_semaphore.semaphore = completion_impl.get_semaphore();
    wait_semaphore.value = completion_impl.get_value();
    wait_semaphore.stageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;
    wait_semaphore.deviceIndex = 0;

    wait_semaphores.push_back(wait_semaphore);
  }

  VkSemaphoreSubmitInfo signal_semaphore = {};
  signal_semaphore.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO;
  signal_semaphore.semaphore = this->timeline;
  signal_semaphore.value = ++this->timeline_value;
  signal_semaphore.stageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;
  signal_semaphore.deviceIndex = 0;

  VkCommandBufferSubmitInfo cmd = {};
  cmd.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO;
  cmd.commandBuffer = command_buffer.get_command_buffer();

  submit_info.commandBufferInfoCount = 1;
  submit_info.pCommandBufferInfos = &cmd;

  submit_info.waitSemaphoreInfoCount = wait_semaphores.size();
  submit_info.pWaitSemaphoreInfos = wait_semaphores.data();

  submit_info.signalSemaphoreInfoCount = 1;
  submit_info.pSignalSemaphoreInfos = &signal_semaphore;

  VkResult res =
      vkQueueSubmit2(this->queue, 1, &submit_info, command_buffer.get_fence());

  if (res != VK_SUCCESS) {
    return Error(1, "vkQueueSubmit2 failed");
  }

  logger->info("Submitted Vulkan command buffer.");

  return Completion::from_impl(
      std::make_unique<CompletionImpl>(this->timeline, this->timeline_value));
}

QueueImpl::~QueueImpl() {
  if (!this->device) return;

  vkDestroySemaphore(this->device, this->timeline, nullptr);

  for (auto &[command_buffer, fence] : this->command_buffer_pool) {
    if (!this->command_pool) return;
    vkFreeCommandBuffers(this->device, this->command_pool, 1, &command_buffer);
    vkDestroyFence(this->device, fence, nullptr);
  }
}

} // namespace fgla::backends::vulkan
