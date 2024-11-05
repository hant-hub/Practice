#include "instance.h"
#include "stdio.h"
#include "vulkan/vulkan.h"
#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"

#define WIDTH 800
#define HEIGHT 900






int main() {
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Day1 Practice", NULL, NULL);
    VulkanContext c = {.window = window};
    InitContext(&c);

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
    }



    DestroyContext(&c);
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
