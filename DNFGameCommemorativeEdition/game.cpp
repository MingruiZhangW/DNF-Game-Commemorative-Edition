#include "game.hpp"
#include "glerrorcheck.hpp"

#include <iostream>

#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include <gtx/io.hpp>

// Constructor
Game::Game() {}

// Destructor
Game::~Game() {}

/*
 * Called once, at program start.
 */
void
Game::init()
{
    // Set the background colour.
    glClearColor(0.3, 0.5, 0.7, 1.0);
}

/*
 * Called once per frame, before guiLogic().
 */
void
Game::appLogic()
{
    // Place per frame, application logic here ...
}

/*
 * Called once per frame, after appLogic(), but before the draw() method.
 */
void
Game::guiLogic()
{}

/*
 * Called once per frame, after guiLogic().
 */
void
Game::draw()
{}

/*
 * Called once, after program is signaled to terminate.
 */
void
Game::cleanup()
{}

/*
 * Event handler.  Handles cursor entering the window area events.
 */
bool
Game::cursorEnterWindowEvent(int entered)
{
    bool eventHandled(false);

    return eventHandled;
}

/*
 * Event handler.  Handles mouse cursor movement events.
 */
bool
Game::mouseMoveEvent(double xPos, double yPos)
{
    bool eventHandled(false);

    return eventHandled;
}

/*
 * Event handler.  Handles mouse button events.
 */
bool
Game::mouseButtonInputEvent(int button, int actions, int mods)
{
    bool eventHandled(false);

    return eventHandled;
}

/*
 * Event handler.  Handles mouse scroll wheel events.
 */
bool
Game::mouseScrollEvent(double xOffSet, double yOffSet)
{
    bool eventHandled(false);

    return eventHandled;
}

/*
 * Event handler.  Handles window resize events.
 */
bool
Game::windowResizeEvent(int width, int height)
{
    bool eventHandled(false);

    return eventHandled;
}

/*
 * Event handler.  Handles key input events.
 */
bool
Game::keyInputEvent(int key, int action, int mods)
{
    bool eventHandled(false);

    return eventHandled;
}
