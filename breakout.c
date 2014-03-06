//
// breakout.c
//
// Computer Science 50
// Problem Set 4
//

// standard libraries
#define _XOPEN_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Stanford Portable Library
#include "gevents.h"
#include "gobjects.h"
#include "gwindow.h"

// height and width of game's window in pixels
#define HEIGHT 600
#define WIDTH 400

// number of rows of bricks
#define ROWS 5

// number of columns of bricks
#define COLS 10

// radius of ball in pixels
#define RADIUS 10

// lives
#define LIVES 3

// prototypes
void initBricks(GWindow window);
GOval initBall(GWindow window);
GRect initPaddle(GWindow window);
GLabel initScoreboard(GWindow window);
GLabel initLives(GWindow window);
void updateScoreboard(GWindow window, GLabel label, int points);
void updateLives(GWindow window, GLabel livecounter, int lives);
GObject detectCollision(GWindow window, GOval ball);
GLabel winScreen(GWindow window);
GLabel perfectScreen(GWindow window);

int main(void)
{
    // seed pseudorandom number generator
    srand48(time(NULL));
    double x_vel = 2.0;
    double y_vel = 2.0;

    // instantiate window
    GWindow window = newGWindow(WIDTH, HEIGHT);

    // instantiate bricks
    initBricks(window);

    // instantiate ball, centered in middle of window
    GOval ball = initBall(window);

    // instantiate paddle, centered at bottom of window
    GRect paddle = initPaddle(window);

    // instantiate scoreboard, centered in middle of window, just above ball
    GLabel label = initScoreboard(window);
    
    // instantiate remain lives
    GLabel livecounter = initLives(window);
    
    // number of bricks initially
    int bricks = COLS * ROWS;

    // number of lives initially
    int lives = LIVES;

    // number of points initially
    int points = 0;
    
    // keep playing until game over
    updateLives(window, livecounter, lives);
    waitForClick();
    while (lives > 0 && bricks > 0)
    {
        // TODO
        // bring up score counter at start
        updateScoreboard(window, label, points);
        // check for mouse event
        GEvent event = getNextEvent(MOUSE_EVENT);

        // if we heard one
        if (event != NULL)
        {
            // if the event was movement
            if (getEventType(event) == MOUSE_MOVED)
            {
                // ensure paddle follows cursor
                double x = getX(event) - getWidth(paddle) / 2;
                setLocation(paddle, x, 550);
            }
        }
        move (ball, x_vel, y_vel);
        if (getX(ball) + getWidth(ball) >= getWidth(window))
        {
            x_vel = -x_vel;
        }
        else if (getX(ball) <= 0)
        {
            x_vel = -x_vel;
        }
        if (getY(ball) + getHeight(ball) >= getHeight(window))
        {
            lives--;
            updateLives(window, livecounter, lives);
            removeGWindow(window, ball);
            removeGWindow(window, paddle);
            pause(500);
            if (lives > 0)
            {
                ball = initBall(window);
                paddle = initPaddle(window);
                waitForClick();
            }
            else
            {
                GLabel game_over = newGLabel("Game Over");
                setFont(game_over, "SanSerif-36");
                setLocation(game_over, 100, 350);
                add(window, game_over);
            }
        }
        else if (getY(ball) <= 0)
        {
            y_vel = -y_vel;
        }
        pause(10);
        GObject object = detectCollision (window, ball);
        if (object == paddle)
        {
            y_vel = -y_vel;
        }
        else if (object != NULL && (strcmp (getType(object), "GRect") == 0))
        {
            bricks--;
            y_vel = -y_vel;
            removeGWindow (window, object);
            points += lives;
        }
        // win if screen is cleared
        if (bricks == 0)
        {
            removeGWindow(window, ball);
            removeGWindow(window, paddle);
            removeGWindow(window, label);
            pause(500);
            if (lives == 3)
            {
                perfectScreen(window);
            }
            else
            {
                winScreen(window);
            }
        }
    }
    // wait for click before exiting
    waitForClick();

    // game over
    closeGWindow(window);
    return 0;
}

/**
 * Initializes window with a grid of bricks.
 */
void initBricks(GWindow window)
{
    // TODO
    int x_pos, y_pos;
    int width = 30;
    int height = 10;
    int y_space = 4;
    for (int i = 0; i < ROWS; i++)
    {
        for (int j = 0; j < COLS; j++)
        {
            x_pos = (j * 40) + 2;
            y_pos = (i * 20) + 3 + y_space;
            GRect brick = newGRect(x_pos, y_pos, width, height);
            switch (i)
            {
                case 0 :
                    setColor(brick, "BLACK");
                    break;
                case 1 :
                    setColor(brick, "RED");
                    break;
                case 2 :
                    setColor(brick, "BLUE");
                    break;
                case 3 :
                    setColor(brick, "GREEN");
                    break;
                case 4 :
                    setColor(brick, "YELLOW");
                    break;
            }
            setFilled(brick, true);
            add(window, brick);
        }
    }
}

/**
 * Instantiates ball in center of window.  Returns ball.
 */
GOval initBall(GWindow window)
{
    // TODO
    GOval ball = newGOval(190, 290, 20, 20);
    setColor(ball, "BLACK");
    setFilled(ball, true);
    add(window, ball);
    return ball;
}

/**
 * Instantiates paddle in bottom-middle of window.
 */
GRect initPaddle(GWindow window)
{
    // TODO
    GRect paddle = newGRect(160, 550, 80, 20);
    setColor(paddle, "BLACK");
    setFilled(paddle, true);
    add(window, paddle);
    return paddle;
}

/**
 * Instantiates, configures, and returns label for scoreboard.
 */
GLabel initScoreboard(GWindow window)
{
    // TODO
    GLabel label = newGLabel("");
    setFont(label, "SansSerif-36");
    add(window, label);
    return label;
}
// Instantiate label for remaining lives
GLabel initLives(GWindow window)
{
    GLabel livecounter = newGLabel("");
    setFont(livecounter, "SansSerif-24");
    setColor(livecounter, "LIGHT_GRAY");
    add(window, livecounter);
    return livecounter;
}

/**
 * Updates scoreboard's label, keeping it centered in window.
 */
void updateScoreboard(GWindow window, GLabel label, int points)
{
    // update label
    char s[12];
    sprintf(s, "%i", points);
    setLabel(label, s);

    // center label in window
    double x = (getWidth(window) - getWidth(label)) / 2;
    double y = (getHeight(window) - getHeight(label)) / 2;
    setLocation(label, x, y);
}
// Update live counter label
void updateLives(GWindow window, GLabel livecounter, int lives)
{
    // update label
    char s[12];
    sprintf(s, "%i", lives);
    setLabel(livecounter, s);

    // print label in bottom-left
    double x = 10;
    double y = (getHeight(window)) - 10;
    setLocation(livecounter, x, y);
}
// Print label for winning
GLabel winScreen(GWindow window)
{
    GLabel you_win = newGLabel("Great job!");
    setFont(you_win, "SanSerif-36");
    double x = (getWidth(window) - getWidth(you_win)) / 2;
    double y = (getHeight(window) - getHeight(you_win)) / 2;
    setLocation(you_win, x, y);
    add(window, you_win);
    return you_win;
}
GLabel perfectScreen(GWindow window)
{
    GLabel perfect = newGLabel("Perfect!");
    setFont(perfect, "SanSerif-36");
    double x = (getWidth(window) - getWidth(perfect)) / 2;
    double y = (getHeight(window) - getHeight(perfect)) / 2;
    setLocation(perfect, x, y);
    add(window, perfect);
    return perfect;
}
/**
 * Detects whether ball has collided with some object in window
 * by checking the four corners of its bounding box (which are
 * outside the ball's GOval, and so the ball can't collide with
 * itself).  Returns object if so, else NULL.
 */
GObject detectCollision(GWindow window, GOval ball)
{
    // ball's location
    double x = getX(ball);
    double y = getY(ball);

    // for checking for collisions
    GObject object;

    // check for collision at ball's top-left corner
    object = getGObjectAt(window, x, y);
    if (object != NULL)
    {
        return object;
    }

    // check for collision at ball's top-right corner
    object = getGObjectAt(window, x + 2 * RADIUS, y);
    if (object != NULL)
    {
        return object;
    }

    // check for collision at ball's bottom-left corner
    object = getGObjectAt(window, x, y + 2 * RADIUS);
    if (object != NULL)
    {
        return object;
    }

    // check for collision at ball's bottom-right corner
    object = getGObjectAt(window, x + 2 * RADIUS, y + 2 * RADIUS);
    if (object != NULL)
    {
        return object;
    }

    // no collision
    return NULL;
}
