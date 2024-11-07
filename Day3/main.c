#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vulkan/vulkan_core.h>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>


typedef struct {
    GLFWwindow* window;
    VkSurfaceKHR surface;
    VkInstance instance;
    VkPhysicalDevice deviceP;
    VkDevice deviceL;
    VkQueue graphics;
    VkQueue present;
    VkSwapchainKHR swapchain;
    VkImage* swapImages;
    uint32_t swapImgNum;
} VulkanContext;

static VulkanContext vk = {0};

const char* validation[] = {
    "VK_LAYER_KHRONOS_validation"
};

const char* Extensions[] = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

void InitVulkan();
void DestroyContext();

int main() {
    glfwInit();
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    vk.window = glfwCreateWindow(900, 600, "Day 3 Practice", NULL, NULL);
    InitVulkan();


    while (!glfwWindowShouldClose(vk.window)) {
        glfwPollEvents();
    }

    DestroyContext();
    glfwDestroyWindow(vk.window);
    glfwTerminate();
    return 0;
}


void InitVulkan() {
    VkApplicationInfo appInfo = {
        .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
        .pApplicationName = "Day 3 Practice",
        .pEngineName = "No Engine",
        .apiVersion = VK_MAKE_VERSION(1, 0, 0),
        .engineVersion = VK_MAKE_VERSION(1, 0, 0),
    };


    uint32_t glfwExtensionCount;
    const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
    VkInstanceCreateInfo instanceInfo = {
        .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        .pApplicationInfo = &appInfo,
        .enabledLayerCount = 1,
        .ppEnabledLayerNames = validation,
        .enabledExtensionCount = glfwExtensionCount,
        .ppEnabledExtensionNames = glfwExtensions,
    };

    if (vkCreateInstance(&instanceInfo, NULL, &vk.instance) != VK_SUCCESS) {
        printf("failed to create Instance\n");
        exit(1);
    }

    if (glfwCreateWindowSurface(vk.instance, vk.window, NULL, &vk.surface) != VK_SUCCESS) {
        printf("Failed to create surface\n");
        exit(1);
    }

    {
        uint32_t deviceCount;
        vkEnumeratePhysicalDevices(vk.instance, &deviceCount, NULL);
        VkPhysicalDevice devices[deviceCount];
        vkEnumeratePhysicalDevices(vk.instance, &deviceCount, devices);

        for (int i = 0; i < deviceCount; i++) {
            VkPhysicalDeviceProperties props;
            vkGetPhysicalDeviceProperties(devices[i], &props);

            uint32_t extensioncount;
            vkEnumerateDeviceExtensionProperties(devices[i], NULL, &extensioncount, NULL);
            VkExtensionProperties extensionProps[extensioncount];
            vkEnumerateDeviceExtensionProperties(devices[i], NULL, &extensioncount, extensionProps);

            uint32_t swapchain = 0;
            for (int i = 0; i < extensioncount; i++) {
                if (strcmp(extensionProps[i].extensionName, Extensions[0]) == 0) {
                    swapchain = 1;
                    break;
                }

            }

            if (props.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU && swapchain) {
                vk.deviceP = devices[i];
                break;
            }
        }


        uint32_t graphics, present;
        
        uint32_t queueCount;
        vkGetPhysicalDeviceQueueFamilyProperties(vk.deviceP, &queueCount, NULL);
        VkQueueFamilyProperties queueProps[queueCount];
        vkGetPhysicalDeviceQueueFamilyProperties(vk.deviceP, &queueCount, queueProps);

        for (int i = 0; i < queueCount; i++) {
            if (queueProps[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
                graphics = i;
                break;
            }
        }
        for (int i = 0; i < queueCount; i++) {
            VkBool32 supported;
            vkGetPhysicalDeviceSurfaceSupportKHR(vk.deviceP, i, vk.surface, &supported);
            if (supported) {
                present = i;
                break;
            }
        }

        float priority = 1.0f;
        uint32_t numQueues = 1;
        VkDeviceQueueCreateInfo queueInfos[2]; 
        queueInfos[0] = (VkDeviceQueueCreateInfo){
            .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
                .queueCount = 1,
                .pQueuePriorities = &priority,
                .queueFamilyIndex = graphics,
        };

        if (present != graphics) {
            numQueues++;
            queueInfos[1] = (VkDeviceQueueCreateInfo) {
                .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
                    .queueCount = 1,
                    .pQueuePriorities = &priority,
                    .queueFamilyIndex = present,
            };
        }

        VkDeviceCreateInfo deviceInfo = {
            .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
            .queueCreateInfoCount = numQueues,
            .pQueueCreateInfos = queueInfos,
            .enabledExtensionCount = 1,
            .ppEnabledExtensionNames = Extensions,
            .enabledLayerCount = 1,
            .ppEnabledLayerNames = validation,
        };

        if (vkCreateDevice(vk.deviceP, &deviceInfo, NULL, &vk.deviceL) != VK_SUCCESS) {
            printf("failed to create Device\n");
            exit(1);
        }
        vkGetDeviceQueue(vk.deviceL, graphics, 0, &vk.graphics);
        vkGetDeviceQueue(vk.deviceL, present, 0, &vk.present);

        //swapchain time >:D
        VkSurfaceCapabilitiesKHR capabilities;
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(vk.deviceP, vk.surface, &capabilities); 

        uint32_t formatCount;
        vkGetPhysicalDeviceSurfaceFormatsKHR(vk.deviceP, vk.surface, &formatCount, NULL);
        VkSurfaceFormatKHR formats[formatCount];
        vkGetPhysicalDeviceSurfaceFormatsKHR(vk.deviceP, vk.surface, &formatCount, formats);

        uint32_t presentModeCount;
        vkGetPhysicalDeviceSurfacePresentModesKHR(vk.deviceP, vk.surface, &presentModeCount, NULL);
        VkPresentModeKHR presentModes[presentModeCount];
        vkGetPhysicalDeviceSurfacePresentModesKHR(vk.deviceP, vk.surface, &presentModeCount, presentModes);

        VkSurfaceFormatKHR format = formats[0];
        for (int i = 0; i < formatCount; i++) {
            if (formats[i].format == VK_FORMAT_R8G8B8_SRGB && formats[i].colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
                format = formats[i];
                break;
            }
        }

        VkPresentModeKHR mode = VK_PRESENT_MODE_FIFO_KHR;
        for (int i = 0; i < presentModeCount; i++) {
            if (presentModes[i] == VK_PRESENT_MODE_MAILBOX_KHR) {
                mode = presentModes[i];
                break;
            }
        }

        
        VkExtent2D extent = {0};
        {
            int width, height;
            glfwGetFramebufferSize(vk.window, &width, &height);

            width = width <= capabilities.maxImageExtent.width ? width: capabilities.maxImageExtent.width; 
            height = height <= capabilities.maxImageExtent.height ? height: capabilities.maxImageExtent.height; 
            
            width = width >= capabilities.minImageExtent.width ? width : capabilities.minImageExtent.width;
            height = height >= capabilities.minImageExtent.height ? height: capabilities.minImageExtent.height; 

            extent = (VkExtent2D){.width = width, .height = height};

        }
        if (capabilities.currentExtent.width != UINT32_MAX) {
            extent = capabilities.currentExtent;
        }

        uint32_t imgCount = capabilities.minImageCount + 1;
        if (capabilities.maxImageCount > 0 && imgCount > capabilities.maxImageCount) {
            imgCount = capabilities.maxImageCount;
        }



        VkSwapchainCreateInfoKHR swapInfo = {
            .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
            .minImageCount = imgCount,
            .surface = vk.surface,
            .imageFormat = format.format,
            .imageColorSpace = format.colorSpace,
            .imageExtent = extent,
            .imageArrayLayers = 1,
            .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
            .preTransform = capabilities.currentTransform,
            .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
            .presentMode = mode,
            .clipped = VK_TRUE,
            .oldSwapchain = VK_NULL_HANDLE
        };

        uint32_t queueIndicies[2] = {graphics, present};
        if (numQueues > 1) {
            swapInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
            swapInfo.queueFamilyIndexCount = 2;
            swapInfo.pQueueFamilyIndices = queueIndicies;
        } else {
            swapInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        }

        if (vkCreateSwapchainKHR(vk.deviceL, &swapInfo, NULL, &vk.swapchain) != VK_SUCCESS) {
            printf("Failed to create Swap chain\n");
            exit(1);
        }

        vkGetSwapchainImagesKHR(vk.deviceL, vk.swapchain, &vk.swapImgNum, NULL);
        vk.swapImages = malloc(sizeof(VkImage) * vk.swapImgNum);
        vkGetSwapchainImagesKHR(vk.deviceL, vk.swapchain, &vk.swapImgNum, vk.swapImages);

        

    }

}

void DestroyContext() {
    free(vk.swapImages);
    vkDestroySwapchainKHR(vk.deviceL, vk.swapchain, NULL);
    vkDestroyDevice(vk.deviceL, NULL);
    vkDestroySurfaceKHR(vk.instance, vk.surface, NULL);
    vkDestroyInstance(vk.instance, NULL);
}
