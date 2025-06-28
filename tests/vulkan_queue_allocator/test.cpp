#include <fgla/backends/vulkan/queue.hpp>
#include <fgla/instance.hpp>
#include <iostream>
#include <bitset>
#include <vulkan/vulkan_core.h>

using namespace fgla::backends::vulkan;

// Helper to print requests
void print_requests(const std::initializer_list<fgla::Queue::Request>& requests) {
    std::cout << "Requests:\n";
    int i = 0;
    for (auto& req : requests) {
	std::cout << "	[" << i++ << "] Type: " << static_cast<int>(req.type) << ", Count: " << req.count << '\n';
    }
}

// Helper to print families
void print_families(const std::vector<VkQueueFamilyProperties>& families) {
    std::cout << "Families:\n";
    int i = 0;
    for (const auto& fam : families) {
	std::cout << "	Index: " << i++ << ", QueueFlags: " << std::bitset<8>(fam.queueFlags) << ", QueueCount: " << fam.queueCount << '\n';
    }
}

// Print results
void print_results(
    const std::vector<VkDeviceQueueCreateInfo>& createInfos,
    const QueueAllocator::QueueMapping& mapping)
{
    std::cout << "Output:\n";
    std::cout << "  QueueCreateInfos count: " << createInfos.size() << '\n';
    for (const auto& info : createInfos) {
	std::cout << "	  FamilyIndex: " << info.queueFamilyIndex << ", QueueCount: " << info.queueCount << '\n';
    }

    std::cout << "  QueueMapping entries: " << mapping.size() << '\n';
    for (const auto& [key, value] : mapping) {
	std::cout << "	  RequestType: " << static_cast<int>(key.first)
		  << ", RequestIndex: " << key.second
		  << " -> FamilyIndex: " << value.first
		  << ", QueueIndex: " << value.second << '\n';
    }
}

void test_big_brain_allocator() {
	// you best believe i didn't write these myself
	{
		std::cout << "=== Test 1: Single request, single family ===\n";
		fgla::Queue::Request r1{fgla::Queue::Type::Graphics, 1};
		std::initializer_list<fgla::Queue::Request> reqs{r1};
		std::vector<VkQueueFamilyProperties> fams{{
		.queueFlags = VK_QUEUE_GRAPHICS_BIT,
		.queueCount = 8
		}};
		print_requests(reqs);
		print_families(fams);
		auto [createInfos, mapping] = QueueAllocator::big_brain_allocator_algorithm(reqs, fams);
		print_results(createInfos, mapping);
	}
	{
		std::cout << "=== Test 2: Multiple requests, multiple families ===\n";
		fgla::Queue::Request r1{fgla::Queue::Type::Graphics, 2};
		fgla::Queue::Request r2{fgla::Queue::Type::Transfer, 1};
		std::initializer_list<fgla::Queue::Request> reqs{r1, r2};
		std::vector<VkQueueFamilyProperties> fams{
		{
		.queueFlags = VK_QUEUE_GRAPHICS_BIT,
		.queueCount = 3 
		},
		{
		.queueFlags = VK_QUEUE_TRANSFER_BIT,
		.queueCount = 1
		}	
		};
		print_requests(reqs);
		print_families(fams);
		auto [createInfos, mapping] = QueueAllocator::big_brain_allocator_algorithm(reqs, fams);
		print_results(createInfos, mapping);
	}
	{
		std::cout << "=== Test 3: Too many requests ===\n";
		fgla::Queue::Request r1{fgla::Queue::Type::Graphics, 3};
		std::initializer_list<fgla::Queue::Request> reqs{r1};
		std::vector<VkQueueFamilyProperties> fams{
		{.queueFlags = VK_QUEUE_GRAPHICS_BIT, .queueCount = 2}
		};
		print_requests(reqs);
		print_families(fams);
		auto [createInfos, mapping] = QueueAllocator::big_brain_allocator_algorithm(reqs, fams);
		print_results(createInfos, mapping);
	}
	{
		std::cout << "=== Test 4: Queue reuse ===\n";
		fgla::Queue::Request r1{fgla::Queue::Type::Graphics, 1};
		fgla::Queue::Request r2{fgla::Queue::Type::Graphics, 1};
		std::initializer_list<fgla::Queue::Request> reqs{r1, r2};
		std::vector<VkQueueFamilyProperties> fams{
		{.queueFlags = VK_QUEUE_GRAPHICS_BIT, .queueCount = 1}
		};
		print_requests(reqs);
		print_families(fams);
		auto [createInfos, mapping] = QueueAllocator::big_brain_allocator_algorithm(reqs, fams);
		print_results(createInfos, mapping);
	}
	{
		std::cout << "=== Test 5: Mixed family support ===\n";
		fgla::Queue::Request r1{fgla::Queue::Type::Graphics, 1};
		fgla::Queue::Request r2{fgla::Queue::Type::Transfer, 1};
		std::initializer_list<fgla::Queue::Request> reqs{r1, r2};
		std::vector<VkQueueFamilyProperties> fams{
		{.queueFlags = VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_TRANSFER_BIT, .queueCount = 2},
		{.queueFlags = VK_QUEUE_TRANSFER_BIT, .queueCount = 1}
		};
		print_requests(reqs);
		print_families(fams);
		auto [createInfos, mapping] = QueueAllocator::big_brain_allocator_algorithm(reqs, fams);
		print_results(createInfos, mapping);
	}
	{
		std::cout << "=== Test 6: Dedicated transfer preferred ===\n";
		fgla::Queue::Request r1{fgla::Queue::Type::Graphics, 1};
		fgla::Queue::Request r2{fgla::Queue::Type::Transfer, 1};
		std::initializer_list<fgla::Queue::Request> reqs{r1, r2};
		std::vector<VkQueueFamilyProperties> fams{
		{.queueFlags = VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_TRANSFER_BIT, .queueCount = 4},
		{.queueFlags = VK_QUEUE_TRANSFER_BIT, .queueCount = 2}
		};
		print_requests(reqs);
		print_families(fams);
		auto [createInfos, mapping] = QueueAllocator::big_brain_allocator_algorithm(reqs, fams);
		print_results(createInfos, mapping);
	}
	{
		std::cout << "=== Test 7: Unknown queue type ===\n";
		fgla::Queue::Request r1{static_cast<fgla::Queue::Type>(999), 1}; // Invalid type
		std::initializer_list<fgla::Queue::Request> reqs{r1};
		std::vector<VkQueueFamilyProperties> fams{
		{.queueFlags = VK_QUEUE_GRAPHICS_BIT, .queueCount = 2}
		};
		print_requests(reqs);
		print_families(fams);
		auto [createInfos, mapping] = QueueAllocator::big_brain_allocator_algorithm(reqs, fams);
		print_results(createInfos, mapping);
	}
}

int main() {
    auto instance = fgla::Instance::create({ nullptr });

    test_big_brain_allocator();
    return 0;
}
