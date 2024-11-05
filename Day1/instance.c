#include "instance.h"
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <vulkan/vulkan_core.h>








void InitContext(VulkanContext* c) {
    VkApplicationInfo appInfo = {
        .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
        .pApplicationName = "Day 1 Practice",
        .apiVersion = VK_MAKE_VERSION(1, 0, 0),
        .pEngineName = "No Engine",
        .engineVersion = VK_MAKE_VERSION(1, 0, 0),
        .applicationVersion = VK_MAKE_VERSION(1, 0, 0)
    };

    uint32_t count;

    VkInstanceCreateInfo instanceInfo = {
        .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        .pApplicationInfo = &appInfo,
        .enabledLayerCount = 0,
        .ppEnabledExtensionNames = glfwGetRequiredInstanceExtensions(&count),
        .enabledExtensionCount = count
    };


    if (vkCreateInstance(&instanceInfo, NULL, &c->instance) != VK_SUCCESS) {
        fprintf(stderr, "failed to create instance\n");
        exit(1);
    }

    glfwCreateWindowSurface(c->instance, c->window, NULL, &c->surface);


    {
        // make physical device
        uint32_t deviceCount = 0;
        vkEnumeratePhysicalDevices(c->instance, &deviceCount, NULL);
        VkPhysicalDevice devices[deviceCount];
        vkEnumeratePhysicalDevices(c->instance, &deviceCount, devices);

        for (int i = 0; i < deviceCount; i++) {
            VkPhysicalDeviceProperties props;
            vkGetPhysicalDeviceProperties(devices[i], &props);
            VkPhysicalDeviceFeatures features;
            vkGetPhysicalDeviceFeatures(devices[i], &features);

            if (1) { //Currently pick first device, replace in future
                c->p = devices[i];
                break;
            }
        }
    }
    {
        uint32_t queueFamilyCount;
        vkGetPhysicalDeviceQueueFamilyProperties(c->p, &queueFamilyCount, NULL);
        VkQueueFamilyProperties props[queueFamilyCount];
        vkGetPhysicalDeviceQueueFamilyProperties(c->p, &queueFamilyCount, props);

        for (int i = 0; i < queueFamilyCount; i++) {
            if (props[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
                c->q.graphics = i;
            }
        }
        for (int i = 0; i < queueFamilyCount; i++) {
            if (props[i].queueFlags) {
                c->q.graphics = i;
            }
        }
    }
    {
        float priorities[] = {1.0f};
        VkDeviceQueueCreateInfo queueInfo = {
                .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
                .queueCount = 1,
                .queueFamilyIndex = c->q.graphics,
                .pQueuePriorities = priorities
        };

        VkPhysicalDeviceFeatures features;
        vkGetPhysicalDeviceFeatures(c->p, &features);
        
        VkDeviceCreateInfo deviceInfo = {
            .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
            .enabledLayerCount = 0,
            .pQueueCreateInfos = &queueInfo,
            .queueCreateInfoCount = 1,
            .pEnabledFeatures = &features,
            .enabledExtensionCount = 0,
            .ppEnabledLayerNames = NULL,
        };

        if (vkCreateDevice(c->p, &deviceInfo, NULL, &c->d) != VK_SUCCESS) {
            fprintf(stderr, "Device failed/n");
            exit(1);
        }

        vkGetDeviceQueue(c->d, c->q.graphics, 0, &c->q.g);
    }
}


void DestroyContext(VulkanContext* c) {
    vkDestroyDevice(c->d, NULL);
    vkDestroySurfaceKHR(c->instance, c->surface, NULL);
    vkDestroyInstance(c->instance, NULL);
}
