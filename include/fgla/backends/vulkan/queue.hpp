#pragma once

#include <fgla/queue.hpp>
#include <vulkan/vulkan.h>
#include <fgla/util.hpp>
#include <unordered_map>

namespace fgla::backends::vulkan {

struct QueueAllocator {
	QueueAllocator(const std::initializer_list<std::reference_wrapper<const Queue::Request>>& requests, VkPhysicalDevice physical_device);
	
	std::unordered_map<std::pair<Queue::Type, uint32_t>, Queue, util::PairHash> get_queues(VkDevice device);
private:
	std::unordered_map<std::pair<Queue::Type, uint32_t>, std::pair<uint32_t, uint32_t>, util::PairHash> queue_mappings; 
};

struct QueueImpl : public fgla::Queue::Impl {
	
private:
	VkQueue queue;
};

}
