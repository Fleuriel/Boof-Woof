/**************************************************************************
 * @file Filepaths.h
 * @author 	TAN Angus Yit Hoe
 * @param DP email: tan.a@digipen.edu [0067684]
 * @co-author Aaron Chan Jun Xiang
 * @par DP email: aaronjunxiang.chan@digipen.edu [2200880]
 * @par Course: CSD 3401
 * @par Game Project 3
 * @date 02-10-2024
 * @brief This header file contains all the filepaths to the respective assets
 *************************************************************************/




#define MICROSOFT_WINDOWS_WINBASE_H_DEFINE_INTERLOCKED_CPLUSPLUS_OVERLOADS 0
#include <windows.h>
#include "WindowManager.h"

#define GLM_FORCE_SILENT_WARNINGS
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "Input/Input.h"

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
    glfwSetScrollCallback(m_Window, ScrollCallBack);
    glfwSetWindowSizeCallback(m_Window, OpenGLWindowResizeCallback);

    glfwSwapInterval(1); // Enable V-Sync

    glEnable(GL_DEPTH_TEST);
    glDepthRange(0.0f, 1.0f);

    std::cout << "Window successfully created." << std::endl;
}

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
    // Update 

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

/**************************************************************************
 * @brief Callback function for handling keyboard input in a GLFW window.
 *
 * This function is a callback used with the GLFW library to handle keyboard input events.
 *
 * @param window The GLFW window that received the input.
 * @param key The keyboard key code that was pressed or released.
 * @param scancode The system-specific scancode of the key.
 * @param action The action taken (GLFW_PRESS, GLFW_RELEASE, GLFW_REPEAT).
 * @param mod Bitfield describing which modifier keys (e.g., Shift, Ctrl, Alt) were held down.
 *
 * This function updates various input states based on the keyboard input events.
 * - When a key is pressed (action == GLFW_PRESS), it updates the keyStates array
 *   to record the state of alphabets, numbers, special keys, and keyboard commands.
 * - Additionally, it can set or clear certain input states based on the key pressed,
 *   such as Caps Lock, Tab, and Escape.
 *
 * @note The UNREFERENCED_PARAMETER macro is used to suppress unused parameter warnings.
 * @note This function is typically registered with GLFW using glfwSetKeyCallback().
 *
 * @see keyStates - The array used to store the state of various keyboard keys.
 * @see glfwSetKeyCallback() - Function to register this callback with GLFW.
 *************************************************************************/
void Window::KeyCallBack(GLFWwindow* window, int key, int scancode, int action, int mods) {
    // Handle key input
    (void)window;
    (void)mods;
    (void)action;
    (void)scancode;



    // Return if unknown key pressed (e.g. multimedia keys)
    if (key == GLFW_KEY_UNKNOWN)
        return;

    if (!g_Input.typePW) {

        /*
        Update the state of the pressed key
         - If the key is pressed (action == GLFW_PRESS) and its state was not previously pressed,
           set its state to 1 (pressed).
         - If the key is released (action == GLFW_RELEASE), set its state to 0 (not pressed).
         - If the key is held down (action == GLFW_REPEAT), set its state to 2 (held down).
        */
        g_Input.SetKeyState(key, (action == GLFW_PRESS && g_Input.GetKeyState(key) == 0) ? 1 : (action == GLFW_RELEASE) ? 0 : 2);


#ifdef _DEBUG
        // Print debug information based on the key action (press, hold, release)
        //std::cout << ((action == GLFW_PRESS) ? "Pressed Keys\n" : (action == GLFW_REPEAT) ? "Held Keys\n" : "Released Keys\n");
#endif

        if (key == GLFW_KEY_CAPS_LOCK) g_Input.capsLockReleased = (action == GLFW_RELEASE) ? true : false;

    }


    if (key == GLFW_KEY_F1 && action == GLFW_PRESS) {
        std::cout << std::endl;
        g_Input.typePW = !g_Input.typePW;
        g_Input.hiddenconsole = "";

       
    }


    if (g_Input.typePW) {
        if (action == GLFW_PRESS) {
            if (key >= GLFW_KEY_A && key <= GLFW_KEY_Z) {
                char newchar{ 'a' + static_cast<char>(key - GLFW_KEY_A) };
                g_Input.hiddenconsole += newchar;
#ifdef _DEBUG
                std::cout << g_Input.hiddenconsole << std::endl;
#endif
            }
            if (key == GLFW_KEY_ENTER) {
                if (g_Input.hiddenconsole == "helloworld") {
                    std::cout << "Good Bye World\n";
                }
                g_Input.hiddenconsole = "";
            }
        }
        if (action == GLFW_RELEASE) {
            if (key == GLFW_KEY_ENTER) {
                g_Input.typePW = false;
                std::cout << "Hidden Console Turned Off\n";
            }
        }
    }

    g_Input.buttonPressed = action;
}

/**************************************************************************
 * @brief Callback function for handling mouse button input in a GLFW window.
 *
 * This function is a callback used with the GLFW library to handle mouse button input events.
 *
 * @param window The GLFW window that received the input.
 * @param button The mouse button that was pressed or released.
 * @param action The action taken (GLFW_PRESS or GLFW_RELEASE).
 * @param mod Bitfield describing which modifier keys (e.g., Shift, Ctrl, Alt) were held down.
 *
 * This function updates the mouseButtonStates array based on mouse button input events. It specifically
 * records the state of the mouse buttons based on whether they are pressed or released.
 *
 * @note The UNREFERENCED_PARAMETER macro is used to suppress unused parameter warnings.
 * @note This function is typically registered with GLFW using glfwSetMouseButtonCallback().
 *
 * @see mouseButtonStates - The array used to store the state of various input events.
 * @see glfwSetMouseButtonCallback() - Function to register this callback with GLFW.
 *************************************************************************/
void Window::MouseCallBack(GLFWwindow* window, int button, int action, int mods) {
    (void)window;
    (void)mods;
    //UNREFERENCED_PARAMETER(window);
    //UNREFERENCED_PARAMETER(mod);

    g_Input.SetMouseState(button, action);

    //std::cout << g_Input.GetMouseState(button);

    g_Input.buttonPressed = action;

}

/**************************************************************************
 * @brief Callback function for handling mouse scroll wheel input in a GLFW window.
 *
 * This function is a callback used with the GLFW library to handle mouse scroll wheel input events.
 *
 * @param window The GLFW window that received the input.
 * @param xOffset The horizontal scroll offset (not used in this function).
 * @param yOffset The vertical scroll offset, indicating the amount of scrolling.
 *
 * This function updates the `mouse_scroll_total_Y_offset` variable to keep track of the total vertical
 * scrolling that has occurred. It also sets the mouseScrollState variable to indicate whether scrolling
 * is in the upward (INPUT_SCROLLUP) or downward (INPUT_SCROLLDOWN) direction based on the `yOffset` value.
 *
 * @param mouse_scroll_total_Y_offset A global variable that tracks the total vertical scrolling.
 * @param mouseScrollState The array used to store the state of various input events.
 *
 * @note The UNREFERENCED_PARAMETER macro is not used in this function.
 * @note This function is typically registered with GLFW using glfwSetScrollCallback().
 *
 * @see mouse_scroll_total_Y_offset - Global variable to track total vertical scrolling.
 * @see mouseScrollState - The array used to store the state of various input events.
 * @see glfwSetScrollCallback() - Function to register this callback with GLFW.
 *************************************************************************/
void Window::ScrollCallBack(GLFWwindow* window, double xOffset, double yOffset) {
    (void)xOffset;
    (void)window;
    //UNREFERENCED_PARAMETER(window);
    //UNREFERENCED_PARAMETER(xOffset);

    //Update variable to track total vertical scrolling
    g_Input.UpdateScrollTotalYOffset(static_cast<float>(yOffset));

    //Change the scroll states based on y offset value
    g_Input.SetScrollState((yOffset > 0) ? 1 : (yOffset == 0) ? 0 : -1);

    //std::cout << g_Input.GetScrollState() << std::endl;
    //std::cout << g_Input.GetScrollTotalYOffset() << std::endl;
}