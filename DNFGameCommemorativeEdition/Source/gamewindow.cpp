#include "gamewindow.hpp"
#include "constant.hpp"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <sstream>
#include <iostream>
#include <cstdio>
#include <ctime>
#include <chrono>
#include <limits.h>

#include <Windows.h>

//-- Static member initialization:
std::string GameWindow::m_exec_dir = ".";
std::chrono::high_resolution_clock::time_point m_game_clock;
std::shared_ptr<GameWindow> GameWindow::m_instance = nullptr;

static void printGLInfo();

//----------------------------------------------------------------------------------------
// Constructor
GameWindow::GameWindow()
    : m_window(nullptr)
    , m_monitor(nullptr)
    , m_windowTitle()
    , m_windowWidth(0)
    , m_windowHeight(0)
    , m_framebufferWidth(0)
    , m_framebufferHeight(0)
    , m_paused(false)
    , m_fullScreen(false)
{}

//----------------------------------------------------------------------------------------
// Destructor
GameWindow::~GameWindow()
{
    // Free all GLFW resources.
    glfwTerminate();
}

//----------------------------------------------------------------------------------------
/*
 * Error callback to be registered with GLFW.
 */
void
GameWindow::errorCallback(int error, const char* description)
{
    std::stringstream msg;
    msg << "GLFW Error Code: " << error << "\n"
        << "GLFW Error Description: " << description << "\n";
    std::cout << msg.str();
}

//----------------------------------------------------------------------------------------
/*
 * Window resize event callback to be registered with GLFW.
 */
void
GameWindow::windowResizeCallBack(GLFWwindow* window, int width, int height)
{
    getInstance()->GameWindow::windowResizeEvent(width, height);
    getInstance()->windowResizeEvent(width, height);
}

//----------------------------------------------------------------------------------------
/*
 * Key input event callback to be registered with GLFW.
 */
void
GameWindow::keyInputCallBack(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (!getInstance()->keyInputEvent(key, action, mods)) {
        // Send event to parent class for processing.
        getInstance()->GameWindow::keyInputEvent(key, action, mods);
    }
}

//----------------------------------------------------------------------------------------
/*
 * Mouse scroll event callback to be registered with GLFW.
 */
void
GameWindow::mouseScrollCallBack(GLFWwindow* window, double xOffSet, double yOffSet)
{
    getInstance()->mouseScrollEvent(xOffSet, yOffSet);
}

//----------------------------------------------------------------------------------------
/*
 * Mouse button event callback to be registered with GLFW.
 */
void
GameWindow::mouseButtonCallBack(GLFWwindow* window, int button, int actions, int mods)
{
    getInstance()->mouseButtonInputEvent(button, actions, mods);
}

//----------------------------------------------------------------------------------------
/*
 *  Mouse move event callback to be registered with GLFW.
 */
void
GameWindow::mouseMoveCallBack(GLFWwindow* window, double xPos, double yPos)
{
    getInstance()->mouseMoveEvent(xPos, yPos);
}

//----------------------------------------------------------------------------------------
/*
 * Cursor enter window event callback to be registered with GLFW.
 */
void
GameWindow::cursorEnterWindowCallBack(GLFWwindow* window, int entered)
{
    getInstance()->cursorEnterWindowEvent(entered);
}

//----------------------------------------------------------------------------------------
/*
 * Event handler. Handles window resize events.
 */
bool
GameWindow::windowResizeEvent(int width, int height)
{
    m_windowWidth = width;
    m_windowHeight = height;

    return false;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler. Handles key input events.
 */
bool
GameWindow::keyInputEvent(int key, int action, int mods)
{
    return false;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler. Handles mouse scroll events.
 */
bool
GameWindow::mouseScrollEvent(double xOffSet, double yOffSet)
{
    return false;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler. Handles mouse button events.
 */
bool
GameWindow::mouseButtonInputEvent(int button, int actions, int mods)
{
    return false;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler. Handles mouse move events.
 */
bool
GameWindow::mouseMoveEvent(double xPos, double yPos)
{
    return false;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler. Handles mouse cursor entering window area events.
 */
bool
GameWindow::cursorEnterWindowEvent(int entered)
{
    return false;
}

//----------------------------------------------------------------------------------------
void
GameWindow::centerWindow()
{
    int windowWidth, windowHeight;
    glfwGetWindowSize(m_window, &windowWidth, &windowHeight);

    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    if (monitor == NULL) {
        return;
    }

    int x, y;
    const GLFWvidmode* videoMode = glfwGetVideoMode(monitor);
    x = videoMode->width;
    y = videoMode->height;

    x = (x - windowWidth) / 2;
    y = (y - windowHeight) / 2;

    glfwSetWindowPos(m_window, x, y);
}

//----------------------------------------------------------------------------------------
/*
 * Register callbacks with GLFW, and associate events with the current GLFWwindow.
 */
void
GameWindow::registerGlfwCallBacks()
{
    glfwSetKeyCallback(m_window, keyInputCallBack);
    glfwSetWindowSizeCallback(m_window, windowResizeCallBack);
    glfwSetScrollCallback(m_window, mouseScrollCallBack);
    glfwSetMouseButtonCallback(m_window, mouseButtonCallBack);
    glfwSetCursorPosCallback(m_window, mouseMoveCallBack);
    glfwSetCursorEnterCallback(m_window, cursorEnterWindowCallBack);
}
//----------------------------------------------------------------------------------------
/*
 * Returns the static instance of class GameWindow.
 */
std::shared_ptr<GameWindow>
GameWindow::getInstance()
{
    static GameWindow* instance = new GameWindow();
    if (m_instance == nullptr) {
        // Pass ownership of instance to shared_ptr.
        m_instance = std::shared_ptr<GameWindow>(instance);
    }
    return m_instance;
}

//----------------------------------------------------------------------------------------
void
GameWindow::launch(int argc,
                   char** argv,
                   GameWindow* window,
                   int width,
                   int height,
                   const std::string& title,
                   float fps)
{
    char* slash = strrchr(argv[0], '/');
    if (slash == nullptr) {
        m_exec_dir = ".";
    } else {
        m_exec_dir = std::string(argv[0], slash);
    }

    if (m_instance == nullptr) {
        m_instance = std::shared_ptr<GameWindow>(window);
        m_instance->run(width, height, title, fps);
    }
}

//----------------------------------------------------------------------------------------
int
GameWindow::getTimeTickInMs()
{
    auto timeTicks = std::chrono::high_resolution_clock::now() - m_game_clock;
    auto msTime = std::chrono::duration_cast<std::chrono::milliseconds>(timeTicks).count();
    if (msTime > INT_MAX - 1000000)
        m_game_clock = std::chrono::high_resolution_clock::now();
    return static_cast<int>(
        std::chrono::duration_cast<std::chrono::milliseconds>(timeTicks).count());
}

//----------------------------------------------------------------------------------------
void
GameWindow::run(int width, int height, const std::string& windowTitle, float desiredFramesPerSecond)
{
    m_windowTitle = windowTitle;
    m_windowWidth = width;
    m_windowHeight = height;
    glfwSetErrorCallback(errorCallback);

    if (glfwInit() == GL_FALSE) {
        fprintf(stderr, "Call to glfwInit() failed.\n");
        std::abort();
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_VISIBLE, GL_TRUE);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    glfwWindowHint(GLFW_SAMPLES, 0);
    glfwWindowHint(GLFW_RED_BITS, 8);
    glfwWindowHint(GLFW_GREEN_BITS, 8);
    glfwWindowHint(GLFW_BLUE_BITS, 8);
    glfwWindowHint(GLFW_ALPHA_BITS, 8);

    m_monitor = glfwGetPrimaryMonitor();
    if (m_monitor == NULL) {
        glfwTerminate();
        fprintf(stderr, "Error retrieving primary monitor.\n");
        std::abort();
    }

    m_window = glfwCreateWindow(width, height, windowTitle.c_str(), NULL, NULL);
    if (m_window == NULL) {
        glfwTerminate();
        fprintf(stderr, "Call to glfwCreateWindow failed.\n");
        std::abort();
    }

    // Get default framebuffer dimensions in order to support high-definition
    // displays.
    glfwGetFramebufferSize(m_window, &m_framebufferWidth, &m_framebufferHeight);

    centerWindow();
    glfwMakeContextCurrent(m_window);

    // Allow modern extension features
    glewExperimental = GL_TRUE;

    if (glewInit() != GLEW_OK) {
        std::cout << ("GLEW initialisation failed!") << std::endl;
        glfwDestroyWindow(m_window);
        glfwTerminate();
        return;
    }

    glfwInit();

#ifdef DEBUG_GL
    printGLInfo();
#endif

    glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

    registerGlfwCallBacks();

    // Make sure resize is run on initialization to get consistent results across platforms
    getInstance()->windowResizeEvent(width, height);

    // Clear error buffer.
    while (glGetError() != GL_NO_ERROR)
        ;

    // Set Window Icon
    GLFWimage icons[1];
    icons[0].pixels = stbi_load(IconPath::iconPath.c_str(), &icons[0].width, &icons[0].height, 0, 4);

    glfwSetWindowIcon(m_window, 1, icons);
    stbi_image_free(icons[0].pixels);

    try {
        // Wait until m_monitor refreshes before swapping front and back buffers.
        // To prevent tearing artifacts.
        glfwSwapInterval(1);

        // Call client-defined startup code.
        init();

        // steady_clock::time_point frameStartTime;
        m_game_clock = std::chrono::high_resolution_clock::now();
        std::clock_t start;
        float duration;

        // Main Program Loop:
        while (!glfwWindowShouldClose(m_window)) {
            // Lap
            start = std::clock();

            glfwPollEvents();

            if (!m_paused) {
                // Apply application-specific logic
                appLogic();

                guiLogic();

                // Ask the derived class to do the actual OpenGL drawing.
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                draw();

                // In case of a window resize, get new framebuffer dimensions.
                glfwGetFramebufferSize(m_window, &m_framebufferWidth, &m_framebufferHeight);

                // Finally, blast everything to the screen.
                glfwSwapBuffers(m_window);
            }

            duration = (std::clock() - start) / static_cast<float>(CLOCKS_PER_SEC);

            if (FPS::frameDelay > duration) {
                Sleep(FPS::frameDelay - duration);
            }
        }

    } catch (const std::exception& e) {
        std::cerr << "Exception Thrown: ";
        std::cerr << e.what() << std::endl;
    } catch (...) {
        std::cerr << "Uncaught exception thrown!  Terminating Program." << std::endl;
    }

    cleanup();
    glfwDestroyWindow(m_window);
}

//----------------------------------------------------------------------------------------
void
GameWindow::init()
{
    // Render only the front face of geometry.
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

    // Setup depth testing
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LEQUAL);
    glDepthRange(0.0f, 1.0f);
    glEnable(GL_DEPTH_CLAMP);

    glClearDepth(1.0f);
    glClearColor(0.3, 0.5, 0.7, 1.0);
}

//----------------------------------------------------------------------------------------
void
GameWindow::appLogic()
{}

//----------------------------------------------------------------------------------------
void
GameWindow::draw()
{}

//----------------------------------------------------------------------------------------
void
GameWindow::guiLogic()
{}

//----------------------------------------------------------------------------------------
void
GameWindow::cleanup()
{}

//----------------------------------------------------------------------------------------
/*
 * Used to print OpenGL version and supported extensions to standard output stream.
 */
static void
printGLInfo()
{
    const GLubyte* renderer = glGetString(GL_RENDERER);
    const GLubyte* vendor = glGetString(GL_VENDOR);
    const GLubyte* version = glGetString(GL_VERSION);
    const GLubyte* glsl_version = glGetString(GL_SHADING_LANGUAGE_VERSION);

    GLint majorVersion, minorVersion;

    glGetIntegerv(GL_MAJOR_VERSION, &majorVersion);
    glGetIntegerv(GL_MINOR_VERSION, &minorVersion);

    std::stringstream message;

    if (vendor)
        message << "GL Vendor          : " << vendor << std::endl;

    if (renderer)
        message << "GL Renderer        : " << renderer << std::endl;

    if (version)
        message << "GL Version         : " << version << std::endl;

    message << "GL Version         : " << majorVersion << "." << minorVersion << std::endl
            << "GLSL Version       : " << glsl_version << std::endl;

    std::cout << message.str();

    std::cout << "Supported Extensions: " << std::endl;
    const GLubyte* extension;
    GLint nExtensions;
    glGetIntegerv(GL_NUM_EXTENSIONS, &nExtensions);

    for (GLuint i = 0; i < nExtensions; ++i) {
        extension = glGetStringi(GL_EXTENSIONS, i);
        if (extension) {
            std::cout << extension << std::endl;
        }
    }
}

std::string
GameWindow::getResourceFilePath(const char* base)
{
    return m_exec_dir + "/Resource/" + base;
}
