#pragma once

#include <cstdint>
#include <fgla/backends/vulkan/adapter.hpp>
#include <fgla/backends/vulkan/memory.hpp>
#include <fgla/backends/vulkan/queue.hpp>
#include <fgla/device.hpp>
#include <fgla/memory.hpp>
#include <unordered_map>
#include <vk_mem_alloc.h>

#if __has_include(<shader-slang/slang-com-ptr.h>)
  #include <shader-slang/slang-com-ptr.h>
  #include <shader-slang/slang.h>
#elif __has_include(<slang-com-ptr.h>)
  #include <slang-com-ptr.h>
  #include <slang.h>
#else
  #error "failed to include slang shader language"
#endif

namespace fgla::backends::vulkan {

struct DeviceImpl : public Device::Impl {
private:
  void init_queue();
  void init_memory();
  void init_shader(const std::vector<std::filesystem::path> &shader_paths);

public:
  DeviceImpl(VkDevice device, VkPhysicalDevice physical_device,
             VkInstance instance, QueueAllocator::Queues queues,
             const std::vector<std::filesystem::path> &shader_paths);

  virtual std::optional<std::reference_wrapper<Queue>>
  get_queue(Queue::Type type, uint32_t index) override;

  virtual Result<ShaderModule>
  load_shader_module(const ShaderModule::Descriptor &desc) override;
  virtual Result<RenderPipeline>
  create_render_pipeline(const RenderPipeline::Descriptor &desc) override;

  virtual Result<Buffer> create_buffer(const Buffer::Descriptor &desc) override;
  virtual Result<Image> create_image(const Image::Descriptor &desc) override;

  VkDevice get_device() const { return this->device; }
  VkPhysicalDevice get_physical_device() const { return this->physical_device; }
  VmaAllocator get_allocator() const { return this->allocator; }
  std::vector<VkSemaphore> &get_semaphore_pool() {
    return this->semaphore_pool;
  }

  // lazy-creates command pools
  VkCommandPool get_command_pool(uint32_t queue_family_index);

  virtual ~DeviceImpl() override;

private:
  VkDevice device;
  VkPhysicalDevice physical_device;
  VkInstance instance;
  QueueAllocator::Queues queues;

  std::vector<uint32_t> used_queue_family_indices;
  VkSharingMode get_resource_sharing_mode() const;

  std::vector<VkSemaphore> semaphore_pool;
  std::unordered_map<uint32_t, VkCommandPool> command_pools;

  VmaAllocator allocator;

  Slang::ComPtr<slang::IGlobalSession> slang_global_session;
  Slang::ComPtr<slang::ISession> slang_session;
};
} // namespace fgla::backends::vulkan
