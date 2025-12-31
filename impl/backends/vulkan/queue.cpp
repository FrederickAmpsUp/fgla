#include <fgla/backends/vulkan/adapter.hpp>
#include <fgla/backends/vulkan/device.hpp>
#include <fgla/backends/vulkan/queue.hpp>
#include <fgla/internal.hpp>
#ifdef FGLA_VK_EXT_WINDOWING
#include <fgla/backends/vulkan/ext/windowing/surface.hpp>
#endif
#include <spdlog/spdlog.h>

namespace std {
template <> struct hash<fgla::Queue::Request> {
  std::size_t operator()(const fgla::Queue::Request &req) const noexcept {
    return (std::hash<int>()(static_cast<int>(req.type)) << 1) ^ std::hash<uint32_t>()(req.count);
  }
};
} // namespace std

namespace fgla::backends::vulkan {

bool operator==(const Queue::Request &lhs, const Queue::Request &rhs) {
  return lhs.type == rhs.type && lhs.count == rhs.count;
}

std::pair<std::vector<VkDeviceQueueCreateInfo>, QueueAllocator::QueueMapping>
QueueAllocator::big_brain_allocator_algorithm(const std::initializer_list<Queue::Request> &requests,
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
      family_infos.push_back(
          {.index = i, .flags = family.queueFlags, .max_queues = family.queueCount});
    }

    ++i;
  }

  std::unordered_map<Queue::Type, std::pair<uint32_t, std::vector<FamilyInfo *>>>
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
            *reinterpret_cast<fgla::ext::windowing::PresentQueueOptions *>(request.user_data);
        VkSurfaceKHR vk_surface =
            dynamic_cast<fgla::backends::vulkan::ext::windowing::SurfaceImpl *>(
                fgla::internal::ImplAccessor::get_impl(present_queue_opts.surface))
                ->get_surface();
        VkPhysicalDevice vk_device =
            dynamic_cast<AdapterImpl *>(
                fgla::internal::ImplAccessor::get_impl(present_queue_opts.adapter))
                ->get_physical_device();

        uint32_t n_queue_families = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(vk_device, &n_queue_families, nullptr);

        VkBool32 present_support = false;
        for (int i = 0; i < n_queue_families; ++i) {
          vkGetPhysicalDeviceSurfaceSupportKHR(vk_device, i, vk_surface, &present_support);
          if (present_support) break;
        }

        supported = present_support;
      } break;
#endif
      default:
        logger->warn("Unknown logical queue type!");
        supported = false;
        break;
      }
      if (supported && family.max_queues > 0) supported_families.push_back(&family);
    }
    if (counts_and_available_families.find(request.type) != counts_and_available_families.end()) {
      counts_and_available_families[request.type].first += request.count;
    } else {
      counts_and_available_families.insert({request.type, {request.count, supported_families}});
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
        final_mapping.insert({{type, i}, {best_family->index, best_family->used_queues.size()}});
        best_family->used_queues.push_back(type);
      } else if (best_family) {
        final_mapping.insert({{type, i}, {best_family->index, idx_fallback_queue}});
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

QueueAllocator::QueueAllocator(const std::initializer_list<Queue::Request> &requests,
                               VkPhysicalDevice physical_device) {
  uint32_t n_queue_families = 0;
  vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &n_queue_families, nullptr);

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

    std::unique_ptr<QueueImpl> queue_impl = std::make_unique<QueueImpl>(vk_queue, queue.first);
    Queue fg_queue = Queue::from_raw(std::move(queue_impl));

    queues.insert({queue_handle, std::move(fg_queue)});
  }

  return queues;
}

} // namespace fgla::backends::vulkan
