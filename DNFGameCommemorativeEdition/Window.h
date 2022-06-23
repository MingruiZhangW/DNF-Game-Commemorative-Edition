#pragma once

#include <stdio.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

class Window
{
public:
    Window();
    Window(GLint windowWidth, GLint windowHeight);

    ~Window();

    int Initialise();

    GLfloat getBufferWidth()
    {
        return bufferWidth_;
    }
    GLfloat getBufferHeight()
    {
        return bufferHeight_;
    }

    bool getShouldClose()
    {
        return glfwWindowShouldClose(mainWindow_);
    }

    void swapBuffers()
    {
        // triple/two buffer (buffer that can be seen)
        glfwSwapBuffers(mainWindow_);
    }

private:
    GLFWwindow* mainWindow_;

    GLint width_ {800}, height_ {600};
    GLint bufferWidth_ {0}, bufferHeight_ {0};
};
