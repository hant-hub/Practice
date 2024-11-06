#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vulkan/vulkan_core.h>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define WIDTH 800
#define HEIGHT 600

void InitVulkan(GLFWwindow* window);

typedef struct {
    VkInstance instance;
    VkPhysicalDevice device_p;
    VkDevice device_l;
    VkQueue graphics;
    VkQueue present;
    VkSurfaceKHR surface;
} VulkanContext;

static VulkanContext vk;

const char* validationlayers[] = {
    "VK_LAYER_KHRONOS_validation"
};

const char* deviceExtensions[] = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME
};



int main() {

    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Day 2 Practice", NULL, NULL);
    InitVulkan(window);


    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}



void InitVulkan(GLFWwindow* window) {
    {
        VkApplicationInfo appInfo = {
            .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
            .apiVersion = VK_MAKE_VERSION(1, 0, 0),
            .pEngineName = "No Engine :)",
            .engineVersion = VK_MAKE_VERSION(1, 0, 0),
            .pApplicationName = "Day 2 Practice",
            .applicationVersion = VK_MAKE_VERSION(1, 0, 0)
        };


        uint32_t extensionCount;
        const char** extensions = glfwGetRequiredInstanceExtensions(&extensionCount);
        VkInstanceCreateInfo instanceInfo = {
            .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
            .pApplicationInfo = &appInfo,
            .enabledLayerCount = 0,
            .ppEnabledLayerNames = NULL,
            .ppEnabledExtensionNames = extensions,
            .enabledExtensionCount = extensionCount
        };

        uint32_t layerCount;
        vkEnumerateInstanceLayerProperties(&layerCount, NULL);
        VkLayerProperties props[layerCount];
        vkEnumerateInstanceLayerProperties(&layerCount, props);


        int found = 0;
        for (int i = 0; i < layerCount; i++) { 
            if (strcmp(validationlayers[0], props[i].layerName) == 0) {
               found = 1; 
               break;
            }
        }

        if (!found) {
            printf("Failed to find validation layers\n");
            exit(1);
        }

        instanceInfo.enabledLayerCount = 1;
        instanceInfo.ppEnabledLayerNames = validationlayers;


        if (vkCreateInstance(&instanceInfo, NULL, &vk.instance) != VK_SUCCESS) {
            printf("Failed to create instance\n");
            exit(1);
        }

        if (glfwCreateWindowSurface(vk.instance, window, NULL, &vk.surface) != VK_SUCCESS) {
            printf("Failed to Create Surface\n");
            exit(1);
        }

    }

    {
        uint32_t deviceCount = 0;
        vkEnumeratePhysicalDevices(vk.instance, &deviceCount, NULL);
        VkPhysicalDevice devices[deviceCount];
        vkEnumeratePhysicalDevices(vk.instance, &deviceCount, devices);
        
        for (int i = 0; i < deviceCount; i++) {
            VkPhysicalDeviceProperties props;
            vkGetPhysicalDeviceProperties(devices[i], &props);


            if (props.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
                vk.device_p = devices[i];
                break;
            }
        }

        //get queue families
        
        uint32_t graphicsQueue;
        uint32_t presentQueue;

        uint32_t queueCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(vk.device_p, &queueCount, NULL);
        VkQueueFamilyProperties queueProperties[queueCount];
        vkGetPhysicalDeviceQueueFamilyProperties(vk.device_p, &queueCount, queueProperties);

        for (int i = 0; i < queueCount; i++) { 
            if (queueProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
                graphicsQueue = i;
                break;
            }
        }

        for (int i = 0; i < queueCount; i++) { 
            VkBool32 presentSupport;
            vkGetPhysicalDeviceSurfaceSupportKHR(vk.device_p, i, vk.surface, &presentSupport);
            if (presentSupport) {
                presentQueue = i;
                break;
            }
        }

        float p = 1.0f;
        VkDeviceQueueCreateInfo queueInfos[2];
        int numQueues = 1;
        queueInfos[0] = (VkDeviceQueueCreateInfo){
            .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
            .queueCount = 1,
            .queueFamilyIndex = graphicsQueue,
            .pQueuePriorities = &p
        };
        if (presentQueue != graphicsQueue) {
            queueInfos[1] = (VkDeviceQueueCreateInfo){
                .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
                .queueCount = 1,
                .queueFamilyIndex = presentQueue,
                .pQueuePriorities = &p
            };
            numQueues += 1;
        }

        VkPhysicalDeviceFeatures deviceFeatures = {0};

        VkDeviceCreateInfo deviceInfo = {
            .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
            .pQueueCreateInfos = queueInfos,
            .queueCreateInfoCount = numQueues,
            .pEnabledFeatures = &deviceFeatures,
            .enabledExtensionCount = 1,
            .ppEnabledExtensionNames = deviceExtensions,
            .enabledLayerCount = 1,
            .ppEnabledLayerNames = validationlayers,
        };

        if (vkCreateDevice(vk.device_p, &deviceInfo, NULL, &vk.device_l) != VK_SUCCESS) {
            printf("Failed to create Device\n");
            exit(1);
        }
        
        vkGetDeviceQueue(vk.device_l, graphicsQueue, 0, &vk.graphics);
        vkGetDeviceQueue(vk.device_l, presentQueue, 0, &vk.present);
    }

    {
        VkSurfaceCapabilitiesKHR capabilities;
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(vk.device_p, vk.surface, &capabilities);

        uint32_t formatCount;
    }

}


void DestroyContext() {
    vkDestroyInstance(vk.instance, NULL);
}
