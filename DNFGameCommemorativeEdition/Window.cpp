#include "Window.h"

Window::Window() {}

Window::Window(GLint windowWidth, GLint windowHeight)
    : width_(windowWidth)
    , height_(windowHeight)
{}

Window::~Window()
{
    glfwDestroyWindow(mainWindow_);
    glfwTerminate();
}

int
Window::Initialise()
{
    // Init GLFW
    if (!glfwInit()) {
        printf("GLFW Init failed!");
        glfwTerminate();
        return 1;
    }

    // Set GLFW properties
    // OpenGL version
    // 3.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    // No backward compatible
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    mainWindow_ = glfwCreateWindow(width_, height_, "Test", NULL, NULL);

    if (!mainWindow_) {
        printf("Window creation failed!");
        glfwTerminate();
        return 1;
    }

    // Get buffer size info (buffer that the actual draw happens)
    glfwGetFramebufferSize(mainWindow_, &bufferWidth_, &bufferHeight_);

    // Set context for GLEW to use
    // (OpenGL context ties/ draws)
    glfwMakeContextCurrent(mainWindow_);

    // Allow modern extension features
    glewExperimental = GL_TRUE;

    // Init GLEW
    if (glewInit() != GLEW_OK) {
        printf("GLEW init failed!");
        glfwDestroyWindow(mainWindow_);
        glfwTerminate();
        return 1;
    }

    // Help us to test which triangle to be drawen at the top of others
    glEnable(GL_DEPTH_TEST);

    // Setup Viewport size (gl)
    // sets up the size of the part we draw to on our window
    // entire window
    glViewport(0, 0, bufferWidth_, bufferHeight_);

    return 0;
}
