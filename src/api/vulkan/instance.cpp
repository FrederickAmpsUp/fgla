#include <fgla/instance.hpp>
#include <vector>
#include <fgla/error.hpp>
#include <fgla/debug.hpp>
#include <optional>
#include <set>
#include <limits>
#include <algorithm>
#include "instance.hpp"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>


namespace fgla {

// unused for now
static VKAPI_ATTR VkBool32 VKAPI_CALL vulkan_debug_callback(
        VkDebugUtilsMessageSeverityFlagBitsEXT severity,
        VkDebugUtilsMessageTypeFlagsEXT type,
        const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
        void* pUserData
    ) {

        // TODO: use debug, warn instead of just error (severity)
    FGLA_ER_PRINTF("Vulkan error - \"%s\"\n", pCallbackData->pMessage);

    return VK_FALSE;
}

struct QueueFamilyIndices {
    std::optional<uint32_t> graphics;
    std::optional<uint32_t> present;
};

struct SwapChainSupportInfo {
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};

QueueFamilyIndices find_queue_families(VkPhysicalDevice dev, VkSurfaceKHR surf) {
    QueueFamilyIndices indices;

    uint32_t nQueueFamilies;
    vkGetPhysicalDeviceQueueFamilyProperties(dev, &nQueueFamilies, nullptr);

    std::vector<VkQueueFamilyProperties> queueFamilies(nQueueFamilies);
    vkGetPhysicalDeviceQueueFamilyProperties(dev, &nQueueFamilies, queueFamilies.data());

    for (int i = 0; i < queueFamilies.size(); ++i) {
        VkQueueFamilyProperties fam = queueFamilies[i];

        if (fam.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            indices.graphics = i;
        }

        VkBool32 presentSupport = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(dev, i, surf, &presentSupport);
        if (presentSupport) {
            indices.present = i;
        }
    }

    return indices;
}

SwapChainSupportInfo query_swapchain_support(VkPhysicalDevice dev, VkSurfaceKHR surf) {
    SwapChainSupportInfo info;

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(dev, surf, &info.capabilities);

    uint32_t nFormats;
    vkGetPhysicalDeviceSurfaceFormatsKHR(dev, surf, &nFormats, nullptr);

    if (nFormats != 0) {
        info.formats.resize(nFormats);
        vkGetPhysicalDeviceSurfaceFormatsKHR(dev, surf, &nFormats, info.formats.data());
    }
        
    uint32_t nPresentModes;
    vkGetPhysicalDeviceSurfacePresentModesKHR(dev, surf, &nPresentModes, nullptr);

    if (nPresentModes != 0) {
        info.presentModes.resize(nPresentModes);
        vkGetPhysicalDeviceSurfacePresentModesKHR(dev, surf, &nPresentModes, info.presentModes.data());
    }

    return info;
}

VkSurfaceFormatKHR pick_swapchain_surface_format(const std::vector<VkSurfaceFormatKHR>& available) {
    VkSurfaceFormatKHR fallback;
    bool hasFallback = false;
    for (VkSurfaceFormatKHR fmt : available) {
        if (fmt.format == VK_FORMAT_B8G8R8A8_SRGB && fmt.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            return fmt;
        }
        if (fmt.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            fallback = fmt;
            hasFallback = true;
        }
    }
    if (hasFallback) return fallback;
    return available[0]; // default to first available format
}

VkPresentModeKHR pick_swapchain_present_mode(const std::vector<VkPresentModeKHR>& available) {
    for (VkPresentModeKHR mode : available) {
        if (mode == VK_PRESENT_MODE_MAILBOX_KHR) {
            return mode; // triple buffered
        }
    }
        // double buffered
    return VK_PRESENT_MODE_FIFO_KHR; // fallback to VK_PRESENT_MODE_FIFO_KHR if mailbox is not available
}


VkExtent2D pick_swapchain_extent(const VkSurfaceCapabilitiesKHR& capabilities, uint32_t desiredWidth, uint32_t desiredHeight) {
    if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
        return capabilities.currentExtent;
    } else {
        int width, height;

        VkExtent2D actualExtent = {
            desiredWidth,
            desiredHeight
        };

        actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
        actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

        return actualExtent;
    }
}
// < 0: not suitable
// > 0: higher the better 
static float device_suitability(VkPhysicalDevice dev, VkSurfaceKHR surf) {
    float s = 0.0;
    QueueFamilyIndices indices = find_queue_families(dev, surf);

    SwapChainSupportInfo swapChainSupport = query_swapchain_support(dev, surf);
    bool swapChainSuitable = (!swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty());

    if (indices.graphics.has_value() && indices.present.has_value() && swapChainSuitable) s += 1.0f;

    return s;
}

Instance::Instance(std::shared_ptr<fgla::Window> win) {
    FGLA_DB_PRINTF("Instance::Instance(win)\n");

    FGLA_DB_PRINTF("Instance creation on window (%u, %u, %s)\n", win->width(), win->height(), win->title().c_str());

    this->m_window = win;
    
    this->m_inf = new NativeInstanceInfo;

    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = win->title().c_str();
    appInfo.pEngineName = "Fred's Graphics Library Abstraction";
    appInfo.engineVersion = VK_MAKE_VERSION(0, 0, 1);
    appInfo.apiVersion = VK_API_VERSION_1_2;

    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;


    std::vector<const char *> requiredExtensions = {
    };

    #ifndef NDEBUG
        static const bool useValidationLayers = true;
        // requiredExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    #else
        static const bool useValidationLayers = false;
    #endif

    FGLA_DB_PRINTF("Enabling %u extensions:\n", requiredExtensions.size());
    for (const char *requiredExtension : requiredExtensions) {
        FGLA_DB_PRINTF("\t- \"%s\"\n", requiredExtension);
    }
    FGLA_DB_PRINTF("\n");

    uint32_t nGLFWExtensions = 0;
    const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&nGLFWExtensions);
    FGLA_DB_PRINTF("GLFW requring %u additional extensions:\n", nGLFWExtensions);

    if (nGLFWExtensions == 0) {
        throw std::runtime_error("GLFW failed to return required Vulkan extensions");
    }

    for (int i = 0; i < nGLFWExtensions; ++i) {
        requiredExtensions.push_back(glfwExtensions[i]);
        FGLA_DB_PRINTF("\t- \"%s\"\n", glfwExtensions[i]);
    }
    FGLA_DB_PRINTF("\n");

    uint32_t nSupportedExtensions = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &nSupportedExtensions, nullptr);
    std::vector<VkExtensionProperties> supportedExtensions(nSupportedExtensions);
    vkEnumerateInstanceExtensionProperties(nullptr, &nSupportedExtensions, supportedExtensions.data());

    FGLA_DB_PRINTF("Vulkan context supports %u extensions:\n", nSupportedExtensions);

    for (VkExtensionProperties ext : supportedExtensions) {
        FGLA_DB_PRINTF("\t- \"%s\"\n", ext.extensionName);
    }
    FGLA_DB_PRINTF("\n");

    std::vector<const char *> unsupportedExtensions(0);
    for (const char *requiredExtension : requiredExtensions) {
        bool sp = false;
            // i'm sure there's a fancy C++ way to do this, but it'll be the same thing anyway.
        for (VkExtensionProperties ext : supportedExtensions) {
            if (std::string(requiredExtension) == std::string(ext.extensionName)) {
                sp = true;
                break;
            }
        }
        if (!sp) unsupportedExtensions.push_back(requiredExtension);
    }

    if (unsupportedExtensions.size() != 0) {
        std::string m = "\nVulkan context does not support " + std::to_string(unsupportedExtensions.size()) + " required extensions:";
        for (const char *unsupportedExtension : unsupportedExtensions) {
            m += "\n\t- \"" + std::string(unsupportedExtension) + "\"";
        }
        throw std::runtime_error(m);
    }

    static const std::vector<const char *> requestedValidationLayers = {
        "VK_LAYER_KHRONOS_validation"
    };

    uint32_t nSupportedLayers;
    vkEnumerateInstanceLayerProperties(&nSupportedLayers, nullptr);
    std::vector<VkLayerProperties> supportedLayers(nSupportedLayers);
    vkEnumerateInstanceLayerProperties(&nSupportedLayers, supportedLayers.data());

    FGLA_DB_PRINTF("Vulkan context supports %u validation layers:\n", nSupportedLayers);
    for (VkLayerProperties layer : supportedLayers) {
        FGLA_DB_PRINTF("\t- \"%s\"\n", layer.layerName);
    }
    FGLA_DB_PRINTF("\n");

    std::vector<const char *> usedLayers(0);

    for (const char *layer : requestedValidationLayers) {
        bool sp = false;
        for (VkLayerProperties supportedLayer : supportedLayers) {
            if (std::string(layer) == std::string(supportedLayer.layerName)) {
                sp = true;
                break;
            }
        }
        if (!sp) {
            FGLA_WN_PRINTF("Validation layer \"%s\" requested but not supported\n", layer);
        } else {
            usedLayers.push_back(layer);
        }
    }

    createInfo.enabledLayerCount = useValidationLayers ? usedLayers.size() : 0;
    createInfo.ppEnabledLayerNames = usedLayers.data();

    createInfo.enabledExtensionCount = requiredExtensions.size();
    createInfo.ppEnabledExtensionNames = requiredExtensions.data();

    VkResult res = vkCreateInstance(&createInfo, nullptr, &this->m_inf->inst);
    if (res != VK_SUCCESS) {
        throw std::runtime_error("Vulkan failed to create instance");
    }
    FGLA_DB_PRINTF("Created Vulkan instance successfully\n");

    FGLA_DB_PRINTF("Creating window surface\n");

    res = glfwCreateWindowSurface(this->m_inf->inst, (GLFWwindow *)this->m_window->handle(), nullptr, &this->m_inf->windowSurface);
    if (res != VK_SUCCESS) {
        throw std::runtime_error("GLFW failed to create window surface");
    }

    FGLA_DB_PRINTF("Created window surface successfully\n");

    FGLA_DB_PRINTF("Searching for physical devices\n");

    uint32_t nPhysDevs;
    vkEnumeratePhysicalDevices(this->m_inf->inst, &nPhysDevs, nullptr);
    std::vector<VkPhysicalDevice> physDevs(nPhysDevs);
    vkEnumeratePhysicalDevices(this->m_inf->inst, &nPhysDevs, physDevs.data());

    FGLA_DB_PRINTF("Found %u physical devices: \n", nPhysDevs);

        // select the most suitable device
    this->m_inf->physDev = VK_NULL_HANDLE;
    float bestScore = 0;
    for (VkPhysicalDevice physDev : physDevs) {
        VkPhysicalDeviceProperties p{};
        vkGetPhysicalDeviceProperties(physDev, &p);
        
        float score = device_suitability(physDev, this->m_inf->windowSurface);
        if (score > bestScore) {
            bestScore = score;
            this->m_inf->physDev = physDev;
        }
        FGLA_DB_PRINTF("\t- \"%s\" (%s)\n", p.deviceName, (score > 0) ? "suitable" : "unsuitable");
    }
    FGLA_DB_PRINTF("\n");
    if (this->m_inf->physDev == VK_NULL_HANDLE) {
        throw std::runtime_error("No suitable GPUs found");
    }

    VkPhysicalDeviceProperties p{};
    vkGetPhysicalDeviceProperties(this->m_inf->physDev, &p);
    FGLA_DB_PRINTF("Selected device \"%s\"\n", p.deviceName);

    FGLA_DB_PRINTF("Creating logical device\n");

    QueueFamilyIndices indices = find_queue_families(this->m_inf->physDev, this->m_inf->windowSurface);

    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;

    std::set<uint32_t> uniqueQueueFamilyIndices = {
        indices.graphics.value(),
        indices.present.value()
    };

    FGLA_DB_PRINTF("Found graphics queue at index %u\n", indices.graphics.value());
    FGLA_DB_PRINTF("Found present queue at index %u\n", indices.present.value());

    for (uint32_t familyIndex : uniqueQueueFamilyIndices) {
        VkDeviceQueueCreateInfo graphicsQueueCreateInfo{};
        graphicsQueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        graphicsQueueCreateInfo.queueFamilyIndex = familyIndex;
        graphicsQueueCreateInfo.queueCount = 1;
        float queuePriority = 1.0f;
        graphicsQueueCreateInfo.pQueuePriorities = &queuePriority;
        queueCreateInfos.push_back(graphicsQueueCreateInfo);
    }

    FGLA_DB_PRINTF("Using %u unique queue families\n", uniqueQueueFamilyIndices.size());

    VkPhysicalDeviceFeatures deviceFeatures{};
    VkDeviceCreateInfo deviceCreateInfo{};
    deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

    deviceCreateInfo.pQueueCreateInfos = queueCreateInfos.data();
    deviceCreateInfo.queueCreateInfoCount = queueCreateInfos.size();

    deviceCreateInfo.pEnabledFeatures = &deviceFeatures;

    std::vector<const char *> requiredDeviceExtensions = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };

    FGLA_DB_PRINTF("Enabling %u device extensions\n", requiredDeviceExtensions.size());
    for (const char *requiredDeviceExtension : requiredDeviceExtensions) {
        FGLA_DB_PRINTF("\t- \"%s\"\n", requiredDeviceExtension);
    }
    FGLA_DB_PRINTF("\n");

    deviceCreateInfo.enabledExtensionCount = requiredDeviceExtensions.size();
    deviceCreateInfo.ppEnabledExtensionNames = requiredDeviceExtensions.data();

    if (useValidationLayers) {
        deviceCreateInfo.enabledLayerCount = static_cast<uint32_t>(usedLayers.size());
        deviceCreateInfo.ppEnabledLayerNames = usedLayers.data();
    } else {
        deviceCreateInfo.enabledLayerCount = 0;
    }

    res = vkCreateDevice(this->m_inf->physDev, &deviceCreateInfo, nullptr, &this->m_inf->logDev);
    if (res != VK_SUCCESS) {
        throw std::runtime_error("Vulkan failed to create logical device");
    }

    FGLA_DB_PRINTF("Created Vulkan logical device successfully\n");

    vkGetDeviceQueue(this->m_inf->logDev, indices.graphics.value(), 0, &this->m_inf->graphicsQueue);
    FGLA_DB_PRINTF("Extracted graphics family\n");
    vkGetDeviceQueue(this->m_inf->logDev, indices.present.value(), 0, &this->m_inf->presentQueue);
    FGLA_DB_PRINTF("Extracted present family\n");

    FGLA_DB_PRINTF("Creating Swapchain\n");

    SwapChainSupportInfo swapChainSupport = query_swapchain_support(this->m_inf->physDev, this->m_inf->windowSurface);

    VkSurfaceFormatKHR surfaceFormat = pick_swapchain_surface_format(swapChainSupport.formats);
    VkPresentModeKHR presentMode = pick_swapchain_present_mode(swapChainSupport.presentModes);
    VkExtent2D extent = pick_swapchain_extent(swapChainSupport.capabilities, this->m_window->width(), this->m_window->height());

    uint32_t nImages = swapChainSupport.capabilities.minImageCount + 1;
    if (swapChainSupport.capabilities.maxImageCount > 0 && nImages > swapChainSupport.capabilities.maxImageCount) {
        nImages = swapChainSupport.capabilities.maxImageCount;
    }

    VkSwapchainCreateInfoKHR swcCreateInfo{};
    swcCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swcCreateInfo.surface = this->m_inf->windowSurface;

    swcCreateInfo.minImageCount = nImages;
    swcCreateInfo.imageFormat = surfaceFormat.format;
    swcCreateInfo.imageColorSpace = surfaceFormat.colorSpace;
    swcCreateInfo.imageExtent = extent;
    swcCreateInfo.imageArrayLayers = 1;
    swcCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    uint32_t queueFamilyIndices[] = {indices.graphics.value(), indices.present.value()};

    if (indices.graphics != indices.present) {
        swcCreateInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        swcCreateInfo.queueFamilyIndexCount = 2;
        swcCreateInfo.pQueueFamilyIndices = queueFamilyIndices;
    } else {
        swcCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    }

    swcCreateInfo.preTransform = swapChainSupport.capabilities.currentTransform;
    swcCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;


    swcCreateInfo.presentMode = presentMode;
    swcCreateInfo.clipped = VK_TRUE;

    swcCreateInfo.oldSwapchain = VK_NULL_HANDLE;

    res = vkCreateSwapchainKHR(this->m_inf->logDev, &swcCreateInfo, nullptr, &this->m_inf->swapchain);
    if (res != VK_SUCCESS) {
        throw std::runtime_error("Vulkan failed to create swapchain");
    }
    FGLA_DB_PRINTF("Swapchain successfully created\n");
    
    vkGetSwapchainImagesKHR(this->m_inf->logDev, this->m_inf->swapchain, &nImages, nullptr);
    this->m_inf->swapchainImages.resize(nImages);
    vkGetSwapchainImagesKHR(this->m_inf->logDev, this->m_inf->swapchain, &nImages, this->m_inf->swapchainImages.data());

    FGLA_DB_PRINTF("Extracted %u images from swapchain\n", nImages);

    this->m_inf->swapchainImageViews.resize(this->m_inf->swapchainImages.size());

    for (int i = 0; i < this->m_inf->swapchainImages.size(); ++i) {
        VkImageViewCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        createInfo.image = this->m_inf->swapchainImages[i];

        createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        createInfo.format = swcCreateInfo.imageFormat;

        createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

        createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        createInfo.subresourceRange.baseMipLevel = 0;
        createInfo.subresourceRange.levelCount = 1;
        createInfo.subresourceRange.baseArrayLayer = 0;
        createInfo.subresourceRange.layerCount = 1;

        res = vkCreateImageView(this->m_inf->logDev, &createInfo, nullptr, &this->m_inf->swapchainImageViews[i]);
        if (res != VK_SUCCESS) {
            throw std::runtime_error("Vulkan failed to create image view");
        }
        FGLA_DB_PRINTF("Created swapchain image view %u\n", i);
    }

    FGLA_DB_PRINTF("exit Instance::Instance\n");
}

Instance::~Instance() {
    FGLA_DB_PRINTF("Instance::~Instance\n");

    // vkDestroyDebugUtilsMessengerEXT(this->m_inf->inst, this->m_inf->dbMessenger, nullptr);
    // FGLA_DB_PRINTF("Destroyed VkDebugUtilsMessengerEXT\n");
    for (VkImageView view : this->m_inf->swapchainImageViews) {
        vkDestroyImageView(this->m_inf->logDev, view, nullptr);
        FGLA_DB_PRINTF("Destroyed VkImageView\n");
    }

    vkDestroySwapchainKHR(this->m_inf->logDev, this->m_inf->swapchain, nullptr);
    FGLA_DB_PRINTF("Destroyed VkSwapchainKHR\n");

    vkDestroyDevice(this->m_inf->logDev, nullptr);
    FGLA_DB_PRINTF("Destroyed VkDevice\n");

    vkDestroySurfaceKHR(this->m_inf->inst, this->m_inf->windowSurface, nullptr);
    FGLA_DB_PRINTF("Destroyed VkSurfaceKHR\n");

    vkDestroyInstance(this->m_inf->inst, nullptr);
    FGLA_DB_PRINTF("Destroyed VkInstance\n");


    delete this->m_inf;
    FGLA_DB_PRINTF("exit Instance::~Instance\n");
}
}