#pragma once

#include <fgla/queue.hpp>
#include <vulkan/vulkan.h>
#include <fgla/util.hpp>
#include <unordered_map>

namespace fgla::backends::vulkan {

struct QueueAllocator {
	using QueueMapping = std::unordered_map<std::pair<Queue::Type, uint32_t>, std::pair<uint32_t, uint32_t>, util::PairHash>;

	static std::pair<std::vector<VkDeviceQueueCreateInfo>, QueueAllocator::QueueMapping> big_brain_allocator_algorithm(const std::initializer_list<std::reference_wrapper<const Queue::Request>>& requests, std::vector<VkQueueFamilyProperties> families);

	QueueAllocator(const std::initializer_list<std::reference_wrapper<const Queue::Request>>& requests, VkPhysicalDevice physical_device);
	
	std::unordered_map<std::pair<Queue::Type, uint32_t>, Queue, util::PairHash> get_queues(VkDevice device);
private:
	QueueMapping queue_mapping; 
};

struct QueueImpl : public fgla::Queue::Impl {
	
private:
	VkQueue queue;
};

}
