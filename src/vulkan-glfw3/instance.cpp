#include "common.hpp"
#include <spdlog/spdlog.h>
#include <vector>

namespace fgla {

Instance::Instance() {
	initialize();
	this->data = new Instance::NativeData;
	
	VkApplicationInfo app_info{};
	app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	app_info.pApplicationName = "FGLA Application";
	app_info.pEngineName = "Fred's Graphics Library Abstraction";
	app_info.apiVersion = VK_API_VERSION_1_0;

	VkInstanceCreateInfo create_info{};
	create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	create_info.pApplicationInfo = &app_info;

	uint32_t glfw_extension_count = 0;
	const char **glfw_extensions = glfwGetRequiredInstanceExtensions(&glfw_extension_count);
	if (glfw_extension_count > 0) {
		spdlog::info("fgla::Instance::Instance: GLFW requiring {} extensions:", glfw_extension_count);
		for (int i = 0; i < glfw_extension_count; ++i) {
			spdlog::info("fgla::Instance::Instance: \t{}", glfw_extensions[i]);
		}
	} else {
		spdlog::info("flga::Instance::Instance: GLFW requiring no extensions. Lucky you!");
	}

	std::vector<const char *> extensions(glfw_extension_count);
	for (int i = 0; i < glfw_extension_count; ++i) {
		extensions[i] = glfw_extensions[i];
	}

	// add other extensions here

	spdlog::info("fgla::Instance::Instance: Using {} extensions.", extensions.size());

	create_info.enabledExtensionCount = extensions.size();
	create_info.ppEnabledExtensionNames = extensions.data();
	
#ifndef NDEBUG
	const char *const validation_layers[] = {
		"VK_LAYER_KHRONOS_validation"
	};
#else
	const char *const validation_layers[];
#endif

	unsigned int supported_layer_count;
	vkEnumerateInstanceLayerProperties(&supported_layer_count, nullptr);

	std::vector<VkLayerProperties> supported_layers(supported_layer_count);
	vkEnumerateInstanceLayerProperties(&supported_layer_count, supported_layers.data());

	bool all_layers_supported = true;

	for (const char *layer : validation_layers) {
		bool layer_found = false;
		for (const auto& layer_props : supported_layers) {
			if (strcmp(layer, layer_props.layerName) == 0) {
				layer_found = true;
				break;
			}
		}
		if (!layer_found) {
			all_layers_supported = false;
			break;
		}
	}

	create_info.enabledLayerCount = sizeof(validation_layers) / sizeof(const char *);
	create_info.ppEnabledLayerNames = validation_layers;

	if (!all_layers_supported) {
		create_info.enabledLayerCount = 0;
		create_info.ppEnabledLayerNames = NULL; // TODO: disable unavailable layers instead of nuking all of them
		spdlog::warn("fgla::Instance::Instance: Requested unsupported validation layer(s)!");
	}

	spdlog::info("fgla::Instance::Instance: Requested {} validation layers.", sizeof(validation_layers) / sizeof(const char *));

	VkResult res = vkCreateInstance(&create_info, nullptr, &this->data->instance);

	if (res != VK_SUCCESS) {
		spdlog::error("fgla::Instance::Instance: vkCreateInstance failed!");
		this->data->ok = false;
	} else {
		spdlog::info("fgla::Instance::Instance: Vulkan instance created successfully.");
		this->data->ok = true;
	}
}

bool Instance::is_ok() {
	return this->data->ok;
}

Instance::~Instance() {
	if (this->is_ok())
		vkDestroyInstance(this->data->instance, nullptr);
	delete this->data;
	spdlog::info("fgla::Instance::~Instance: Cleaned up.");
}
}
