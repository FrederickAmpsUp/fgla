#pragma once

#include <fgla/instance.hpp>
#include <vulkan/vulkan.h>
#include <vector>

namespace fgla {

struct Instance::NativeInstanceInfo {
    // VkDebugUtilsMessengerEXT dbMessenger;
    VkInstance inst;
    VkPhysicalDevice physDev;
    VkDevice logDev;
    VkQueue graphicsQueue;
    VkQueue presentQueue;
    VkSurfaceKHR windowSurface;
    VkSwapchainKHR swapchain;
    std::vector<VkImage> swapchainImages;
    std::vector<VkImageView> swapchainImageViews;
};

}