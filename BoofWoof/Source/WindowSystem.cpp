#include "WindowSystem.h"





WindowSystem::WindowSystem() {
    // Initialization if needed
}

WindowSystem::~WindowSystem() {
    // Clean up if needed
}

void WindowSystem::initWindow(WindowComponent& windowComponent) {
    std::cout << "Initializing OpenGL Window Settings\n";

    windowComponent.windowSize.first = 1920;
    windowComponent.windowSize.second = 1080;

    windowComponent.window = glfwCreateWindow(
        windowComponent.windowSize.first,
        windowComponent.windowSize.second,
        "Assignment 1",
        NULL,
        NULL
    );

    if (!windowComponent.window) {
        glfwTerminate();
        std::cerr << "Failed to create GLFW window." << std::endl;
        return;
    }

    // Tell GLFW we are using OpenGL 4.5
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Set the current context
    glfwMakeContextCurrent(windowComponent.window);
    glViewport(0, 0, windowComponent.windowSize.first, windowComponent.windowSize.second);

    // Set callbacks
    glfwSetKeyCallback(windowComponent.window, KeyCallBack);
    glfwSetMouseButtonCallback(windowComponent.window, MouseCallBack);
    glfwSetWindowSizeCallback(windowComponent.window, OpenGLWindowResizeCallback);

    glfwSwapInterval(1); // Enable V-Sync

    glEnable(GL_DEPTH_TEST);
    glDepthRange(0.0f, 1.0f);


    std::cout << "Window successfully created." << std::endl;
}

void WindowSystem::OpenGLWindowResizeCallback(GLFWwindow* window, int width, int height) {
    // Retrieve the WindowComponent associated with this GLFWwindow (optional if using multiple windows)
    // You'd typically store a mapping between GLFWwindow* and WindowComponent if needed.

    // Update the viewport size
    glViewport(0, 0, width, height);

    // If you want to update the component's size, you would do it here:
    // Find the appropriate WindowComponent and update its size
}

void WindowSystem::KeyCallBack(GLFWwindow* window, int key, int scancode, int action, int mods) {
    // Handle key input
}

void WindowSystem::MouseCallBack(GLFWwindow* window, int button, int action, int mods) {
    // Handle mouse input
}