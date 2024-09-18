
#define MICROSOFT_WINDOWS_WINBASE_H_DEFINE_INTERLOCKED_CPLUSPLUS_OVERLOADS 0
#include <windows.h>
#include "WindowSystem.h"

Window* g_Window = nullptr;
int g_WindowX, g_WindowY;
int g_DesktopWidth, g_DesktopHeight;
bool g_WindowClosed;

static void error_callback(int error, const char* description)
{
    static_cast<void>(error);
    fprintf(stderr, "Error: %s\n", description);
}

void SetResolution(int width, int height)
{
    glfwSetWindowSize(g_Window->GetGLFWWindow(), width, height);;
}

// When losing focus
void WindowFocusCallback(GLFWwindow* window, int focused) 
{
    static_cast<void>(window);
    if (focused == GLFW_FALSE) 
    {
        SetResolution(g_DesktopWidth, g_DesktopHeight);
    }
    else {
        SetResolution(g_WindowX, g_WindowY); // Set your game resolution here
    }
}

// Function for key callback
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) 
{
    static_cast<void>(scancode);
    if (key == GLFW_KEY_TAB && mods == GLFW_MOD_ALT && action == GLFW_PRESS) 
    {
        // Alt + Tab is pressed, minimize the window
        glfwIconifyWindow(window);
    }
}

void Window::OnInitialize()
{
    std::cout << "Initializing OpenGL Window Settings\n";

    // Initialising GLFW window
    if (!glfwInit())
    {
        m_IsInitialized = false;
        g_WindowClosed = true;

        exit(EXIT_FAILURE);
    }

    // Tell GLFW we are using OpenGL 4.5
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Define the custom video mode
    GLFWvidmode customVideoMode;
    customVideoMode.width = g_WindowX;
    customVideoMode.height = g_WindowY;
    customVideoMode.redBits = 8; // Customize according to your needs
    customVideoMode.greenBits = 8;
    customVideoMode.blueBits = 8;

    // Create the GLFW window
    //m_Window = glfwCreateWindow(customVideoMode.width, customVideoMode.height, m_Title, glfwGetPrimaryMonitor(), nullptr); //this is for fullscreen
    m_Window = glfwCreateWindow(m_Width, m_Height, m_Title, nullptr, nullptr);

    if (!m_Window)
    {
        glfwTerminate();
        m_IsInitialized = false;
        g_WindowClosed = true;
        exit(EXIT_FAILURE);
    }

    // Set the current context
    glfwMakeContextCurrent(m_Window);
    glViewport(0, 0, static_cast<GLsizei>(m_Width), static_cast<GLsizei>(m_Height));

    // Set callbacks
    glfwSetKeyCallback(m_Window, KeyCallBack);
    glfwSetMouseButtonCallback(m_Window, MouseCallBack);
    glfwSetWindowSizeCallback(m_Window, OpenGLWindowResizeCallback);

    glfwSwapInterval(1); // Enable V-Sync

    glEnable(GL_DEPTH_TEST);
    glDepthRange(0.0f, 1.0f);

    std::cout << "Window successfully created." << std::endl;
}

//void WindowSystem::initWindow(WindowComponent& windowComponent) {
//
//   /* windowComponent.windowSize.first = 1920;
//    windowComponent.windowSize.second = 1080;
//
//    windowComponent.window = glfwCreateWindow(
//        windowComponent.windowSize.first,
//        windowComponent.windowSize.second,
//        "Assignment 1",
//        NULL,
//        NULL
//    );*/
//
//    //if (!windowComponent.window) {
//    //    glfwTerminate();
//    //    std::cerr << "Failed to create GLFW window." << std::endl;
//    //    return;
//    //}
//
//    //// Tell GLFW we are using OpenGL 4.5
//    //glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
//    //glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
//    //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
//
//    //// Set the current context
//    //glfwMakeContextCurrent(windowComponent.window);
//    //glViewport(0, 0, windowComponent.windowSize.first, windowComponent.windowSize.second);
//
//    //// Set callbacks
//    //glfwSetKeyCallback(windowComponent.window, KeyCallBack);
//    //glfwSetMouseButtonCallback(windowComponent.window, MouseCallBack);
//    //glfwSetWindowSizeCallback(windowComponent.window, OpenGLWindowResizeCallback);
//
//    //glfwSwapInterval(1); // Enable V-Sync
//
//    //glEnable(GL_DEPTH_TEST);
//    //glDepthRange(0.0f, 1.0f);
//}

void Window::OpenGLWindowResizeCallback(GLFWwindow* window, int width, int height) {
    // Retrieve the WindowComponent associated with this GLFWwindow (optional if using multiple windows)
    // You'd typically store a mapping between GLFWwindow* and WindowComponent if needed.

    // Update the viewport size
    glViewport(0, 0, width, height);

    // If you want to update the component's size, you would do it here:
    // Find the appropriate WindowComponent and update its size
}

GLFWwindow* Window::GetGLFWWindow()
{
    return m_Window;
}


void Window::OnUpdate()
{
    // Update FPS
    OnUpdateFPS(1.0);

    // no of frames if u ever need it
    m_FrameCount++;

    std::stringstream ss;
    ss << std::fixed;
    ss.precision(2);
    ss << m_Title;
#ifdef _DEBUG
    ss << " | " << "FPS: " << g_Window->GetFPS();
#endif
    glfwSetWindowTitle(m_Window, ss.str().c_str());

    // Swap Buffers and Poll the events
    glfwSwapBuffers(g_Window->GetGLFWWindow());
    glfwPollEvents();

    // full screen ?
}

void Window::OnShutdown()
{
    if (m_IsInitialized)
    {
        glfwDestroyWindow(m_Window);
        glfwTerminate();
        g_WindowClosed = true;
    }
}

void Window::SetWindowTitle(const char* name)
{
    m_Title = name;
}

void Window::OnUpdateFPS(double interval)
{
    // get elapsed time (in seconds) between previous and current frames
    static double prevTime = glfwGetTime();
    double currTime = glfwGetTime();
    m_DeltaTime = currTime - prevTime;
    prevTime = currTime;

    // fps calculations
    static double count = 0.0; // number of game loop iterations
    static double startTime = glfwGetTime();
    // get elapsed time since very beginning (in seconds) ...
    double elapsedTime = currTime - startTime;

    ++count;

    // update fps at least every 10 seconds ...
    interval = (interval < 0.0) ? 0.0 : interval;
    interval = (interval > 10.0) ? 10.0 : interval;
    if (elapsedTime > interval) {
        m_FPS = count / elapsedTime;
        startTime = currTime;
        count = 0.0;
    }
}

float Window::GetAspectRatio()
{
    return static_cast<float>(m_Width) / static_cast<float>(m_Height);
}

void Window::KeyCallBack(GLFWwindow* window, int key, int scancode, int action, int mods) {
    // Handle key input
}

void Window::MouseCallBack(GLFWwindow* window, int button, int action, int mods) {
    // Handle mouse input
}