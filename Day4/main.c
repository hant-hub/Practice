#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vulkan/vulkan_core.h>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>


typedef struct {
    VkPhysicalDevice p;
    VkDevice l;
    VkQueue graphics;
    VkQueue present;
} device;

typedef struct {
    GLFWwindow* window;
    VkInstance instance;
    VkSurfaceKHR surface;
    device d;
} RenderState;

static const char* validation_Layers[] = {
    "VK_LAYER_KHRONOS_validation"
};

static const char* extension_names[] = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME
};





static RenderState r;

void CreateInstance();
void CreateDevice();

int main() {
    glfwInit();
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

     r.window = glfwCreateWindow(900, 600, "Day 4 Practice", NULL, NULL);

    CreateInstance();
    CreateDevice();




    while (!glfwWindowShouldClose(r.window)) {
        glfwPollEvents();
    }


    glfwDestroyWindow(r.window);
    glfwTerminate();
}



void CreateInstance() {
    VkApplicationInfo appInfo = {
        .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
        .apiVersion = VK_MAKE_VERSION(1, 0, 0),
        .pEngineName = "No engine :)",
        .engineVersion = VK_MAKE_VERSION(1, 0, 0),
        .pApplicationName = "Day 4 Practice",
        .applicationVersion = VK_MAKE_VERSION(1, 0, 0)
    };

    uint32_t glfwCount;
    const char** glfwnames = glfwGetRequiredInstanceExtensions(&glfwCount);

    VkInstanceCreateInfo instanceInfo = {
        .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        .pApplicationInfo = &appInfo,
        .enabledLayerCount = 1,
        .ppEnabledLayerNames = validation_Layers,
        .enabledExtensionCount = glfwCount,
        .ppEnabledExtensionNames = glfwnames,
    };

    if (vkCreateInstance(&instanceInfo, NULL, &r.instance) != VK_SUCCESS) {
        printf("Failed to create Instance\n");
        exit(1);
    }

    if (glfwCreateWindowSurface(r.instance, r.window, NULL, &r.surface) != VK_SUCCESS) {
        printf("Failed to create surface\n");
        exit(1);
    }
}


void CreateDevice() {
    uint32_t DeviceCount;
    vkEnumeratePhysicalDevices(r.instance, &DeviceCount, NULL);
    VkPhysicalDevice devices[DeviceCount];
    vkEnumeratePhysicalDevices(r.instance, &DeviceCount, devices);

    for (int i = 0; i < DeviceCount; i++) {
        VkPhysicalDeviceProperties dprops;
        vkGetPhysicalDeviceProperties(devices[i], &dprops);

        if (dprops.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
            r.d.p = devices[i];
            break;
        }
    }


    uint32_t graphics, present;
    uint32_t queueCount;
    vkGetPhysicalDeviceQueueFamilyProperties(r.d.p, &queueCount, NULL);
    VkQueueFamilyProperties queueProps[queueCount];
    vkGetPhysicalDeviceQueueFamilyProperties(r.d.p, &queueCount, queueProps);

    for (int i = 0; i < queueCount; i++) {
        if (queueProps[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            graphics = i;
            break;
        }
    }

    for (int i = 0; i < queueCount; i++) {
        VkBool32 presentable;
        vkGetPhysicalDeviceSurfaceSupportKHR(r.d.p, i, r.surface, &presentable);
        if (presentable) {
            present = i;
            break;
        }
    }


    float priority_temp = 1.0f;
    uint32_t numQueues = 1;
    VkDeviceQueueCreateInfo queueInfos[2];
    queueInfos[0] = (VkDeviceQueueCreateInfo) {
        .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
        .queueCount = 1,
        .queueFamilyIndex = graphics,
        .pQueuePriorities = &priority_temp
    };
    if (graphics != present) {
        numQueues++;
        queueInfos[1] = (VkDeviceQueueCreateInfo) {
            .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
                .queueCount = 1,
                .queueFamilyIndex = present,
                .pQueuePriorities = &priority_temp
        };
    }



    VkDeviceCreateInfo deviceInfo = {
        .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        .enabledExtensionCount = 1,
        .ppEnabledExtensionNames = extension_names,
        .enabledLayerCount = 1,
        .ppEnabledLayerNames = validation_Layers,
        .queueCreateInfoCount = numQueues,
        .pQueueCreateInfos = queueInfos,
    };

    if (vkCreateDevice(r.d.p, &deviceInfo, NULL, &r.d.l) != VK_SUCCESS) {
        printf("Failed to create Device\n");
        exit(1);
    }

    vkGetDeviceQueue(r.d.l, graphics, 0, &r.d.graphics);
    vkGetDeviceQueue(r.d.l, present, 0, &r.d.present);
}
