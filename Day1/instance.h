#ifndef INSTANCE_H
#define INSTANCE_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan_core.h>


typedef struct {
    uint32_t graphics;
    uint32_t present;
    VkQueue g;
    VkQueue p;
} Queues;


typedef struct {
    GLFWwindow* window;
    VkInstance instance;
    VkSurfaceKHR surface;
    VkPhysicalDevice p;
    Queues q;
    VkDevice d;
} VulkanContext;



void InitContext(VulkanContext* c);
void DestroyContext(VulkanContext* c);



#endif
