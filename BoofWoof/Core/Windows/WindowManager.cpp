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

#ifdef APIENTRY
#undef APIENTRY
#endif

#include "pch.h"

#include "WindowManager.h"
#include "Input/Input.h"
#include "Windows.h"
#include "AssetManager/FilePaths.h"

Window* g_Window = nullptr;
int g_WindowX, g_WindowY;
int g_DesktopWidth, g_DesktopHeight;
bool g_WindowClosed;

////////// Settings panel //////////
float resolutionwidth = 1920;
float resolutionheight = 1080;
bool force_resolution = false;
bool fullscreen_on_start = false;
bool out_of_focus_minimize = false;
//bool start_in_game = true;
////////////////////////////////////

DEVMODE devMode;
DEVMODE originalDevMode;

static void error_callback(int error, const char* description)
{
    static_cast<void>(error);
    fprintf(stderr, "Error: %s\n", description);
}

void SetResolution(int width, int height)
{
    glfwSetWindowSize(g_Window->GetGLFWWindow(), width, height);;
}

//// When losing focus
//void WindowFocusCallback(GLFWwindow* window, int focused) 
//{
//    static_cast<void>(window);
//    if (focused == GLFW_FALSE) 
//    {
//        SetResolution(g_DesktopWidth, g_DesktopHeight);
//    }
//    else {
//        SetResolution(g_WindowX, g_WindowY); // Set your game resolution here
//    }
//}

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
    glfwSetErrorCallback(error_callback);



    // Store the original screen settings
    EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &originalDevMode);

    if (force_resolution) {
        // Retrieve current display settings
        if (EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &devMode)) {
            // Check if the current resolution is already right
            if (devMode.dmPelsWidth == resolutionwidth && devMode.dmPelsHeight == resolutionheight) {
                std::cout << "Current resolution is already correct. No change needed." << std::endl;
            }
            else {
                // Change resolution if it is not already 1920x1080
                devMode.dmPelsWidth = static_cast<DWORD>(resolutionwidth);
                devMode.dmPelsHeight = static_cast<DWORD>(resolutionheight);
                devMode.dmFields = DM_PELSWIDTH | DM_PELSHEIGHT;

                if (ChangeDisplaySettings(&devMode, CDS_FULLSCREEN) != DISP_CHANGE_SUCCESSFUL) {
                    std::cerr << "Resolution change failed." << std::endl;
                }
                else {
                    std::cout << "Resolution successfully changed." << std::endl;
                }
            }
        }
        else {
            std::cerr << "Failed to retrieve current display settings." << std::endl;
        }
    }



    glfwSetWindowFocusCallback(m_Window, windowFocusCallback);

    glfwSwapInterval(1); // Enable V-Sync

    glEnable(GL_DEPTH_TEST);
    glDepthRange(0.0f, 1.0f);

    std::cout << "Window successfully created." << std::endl;

    if (fullscreen_on_start)
        toggleFullScreen();
}

void Window::OpenGLWindowResizeCallback(GLFWwindow* window, int width, int height) 
{
    (void)window;
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
    // Calculate deltaTime
    static double lastTime = glfwGetTime(); // Store the time of the last frame
    double currentTime = glfwGetTime();      // Get the current time
    m_DeltaTime = currentTime - lastTime;    // Calculate the time difference (deltaTime)
    lastTime = currentTime;                  // Update the last time for the next frame

    // Update FPS and frame count
    OnUpdateFPS(m_DeltaTime);

    // Increment frame count for each update
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

    // Monitor asset loading periodically
    m_AssetManagerMonitoringTimer += static_cast<float>(m_DeltaTime);

    if (g_AssetManager.Currentlyloading == false && m_AssetManagerMonitoringTimer > 1.f) {
        // Use the defined wide string paths
        g_AssetManager.MonitorFiles(FILEPATH_ASSET_TEXTURES_W);
        //g_AssetManager.MonitorFiles(FILEPATH_ASSET_SPRITES_W);
        g_AssetManager.MonitorFiles(FILEPATH_ASSET_SCENES_W);
        g_AssetManager.MonitorFiles(FILEPATH_ASSET_OBJECTS_W);
        g_AssetManager.MonitorFiles(FILEPATH_ASSET_SHADERS_W);
        g_AssetManager.MonitorFiles(FILEPATH_ASSET_FONTS_W);

        m_AssetManagerMonitoringTimer = 0.f;
    }


		glfwGetWindowSize(m_Window, &g_WindowX, &g_WindowY);

	
}

void Window::OnShutdown()
{

    if (force_resolution)
        // Reset the screen resolution to the original settings
        ChangeDisplaySettings(&originalDevMode, CDS_RESET);

    if (m_IsInitialized)
    {
        glfwDestroyWindow(m_Window);
        glfwTerminate();
        g_WindowClosed = true;
        delete g_Window;
        g_Window = nullptr;
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
 * @brief This function checks if the GLFW window is currently in
          fullscreen mode.
 *************************************************************************/
bool Window::isFullScreen(GLFWwindow* widow) {
    return (glfwGetWindowMonitor(widow) != nullptr);
}

/**************************************************************************
 * @brief Toggles the fullscreen state of the GLFW window.
 *
 * This function checks if the GLFW window is currently in fullscreen mode.
 * If it is not in fullscreen mode, it makes the window fullscreen by setting
 * it to the primary monitor's dimensions. It also stores the window dimensions
 * and position before going fullscreen to restore them when switching back to
 * windowed mode.
 *
 * If the window is already in fullscreen mode, it switches it back to windowed
 * mode using the stored dimensions and position.
 *************************************************************************/

int windowedWidth{}, windowedHeight{}, windowedXPos{}, windowedYPos{};
bool fullScreen{};

void Window::toggleFullScreen() {
    // Toggle fullscreen state
    if (!isFullScreen(m_Window)) {
        // If not fullscreen, make it fullscreen
        GLFWmonitor* monitor = glfwGetPrimaryMonitor();
        const GLFWvidmode* mode = glfwGetVideoMode(monitor);

        // Store window dimensions before going fullscreen
        glfwGetWindowSize(m_Window, &windowedWidth, &windowedHeight);
        glfwGetWindowPos(m_Window, &windowedXPos, &windowedYPos);

        glfwSetWindowMonitor(m_Window, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);

        fullScreen = true;
    }
    else {
        // If fullscreen, make it windowed
        //GLFWmonitor* primaryMonitor = glfwGetPrimaryMonitor();
        //const GLFWvidmode* primaryMode = glfwGetVideoMode(primaryMonitor);

        glfwSetWindowMonitor(m_Window, nullptr, windowedXPos, windowedYPos, windowedWidth, windowedHeight, GLFW_DONT_CARE);

        fullScreen = false;
    }
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

    if (key == GLFW_KEY_F11 && action == GLFW_PRESS)
        g_Window->toggleFullScreen();

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

/**************************************************************************
 * @brief Callback function for handling window focus changes.
 *
 * This function is registered as a callback to be called when the focus of the
 * GLFW window changes. It is designed to respond to the window losing or gaining focus,
 * particularly when the user alt-tabs away from or back to the window.
 *
 * If the window loses focus (alt-tabbed away), it checks whether certain conditions
 * are met (not currently loading assets or the file browser not being open), and
 * if so, it minimizes the window and pauses all sounds using GLFW's `glfwIconifyWindow`
 * and a sound manager.
 *
 * If the window gains focus (alt-tabbed back), it restores the window using `glfwRestoreWindow`
 * and resumes all sounds through the sound manager.
 *
 * @param window The GLFW window that triggered the callback.
 * @param focused An integer indicating whether the window has gained (GLFW_TRUE) or lost (GLFW_FALSE) focus.
 *************************************************************************/
void Window::windowFocusCallback(GLFWwindow* window, int focused) {
	window;

	if (out_of_focus_minimize)
	// If alt tabbed away
	if (focused == GLFW_FALSE) {
		// Minimizes window if alt tabbed away
		if (!(g_AssetManager.Currentlyloading))
			glfwIconifyWindow(window);
		ChangeDisplaySettings(&originalDevMode, CDS_RESET);
	}
	// If alt tabbed back to window
	else {
		// Resores window if alt tabbed back
		glfwRestoreWindow(window);
		ChangeDisplaySettings(&devMode, CDS_FULLSCREEN);
	}
}
