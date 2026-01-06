#include <stdlib.h>
#include <stdio.h>
#include "robot.h"
#include "arena.h"
#include "graphics.h"

static struct {
    int triangleX[3];
    int triangleY[3];
    int triangleOrientation;
    int r1, r2; // robot pos
    int markerCount;
} robot;

// variables in arena.c accessible across files
extern Cell grid[MAX_GRID_SIZE][MAX_GRID_SIZE];
extern int HORIZONTAL_SQUARES, VERTICAL_SQUARES;
extern float whiteRectX, whiteRectY, squareSize;
extern int homeX, homeY;

void initializeRobot(int startX, int startY) {
    robot.r1 = startX;
    robot.r2 = startY;
    robot.markerCount = 0;
    robot.triangleOrientation = RIGHT;
}

int canMoveForward() {
    int nextR1 = robot.r1;
    int nextR2 = robot.r2;

    switch (robot.triangleOrientation) {
        case UP:
            nextR2--;
            break;
        case DOWN:
            nextR2++;
            break;
        case LEFT:
            nextR1--;
            break;
        case RIGHT:
            nextR1++;
            break;
    }

    // check bounds
    if (nextR1 < 0 || nextR1 >= HORIZONTAL_SQUARES || nextR2 < 0 || nextR2 >= VERTICAL_SQUARES) {
        return 0;
    }

    if (grid[nextR1][nextR2].cellType == WALL || grid[nextR1][nextR2].cellType == OBSTACLE) {
        return 0;
    }

    return 1;
}

void forward() {
    if (!canMoveForward()) {
        return;
    }

    grid[robot.r1][robot.r2].hasRobot = 0;

    // update position based on orientation
    switch (robot.triangleOrientation) {
        case UP:
            robot.r2--;
            break;
        case DOWN:
            robot.r2++;
            break;
        case LEFT:
            robot.r1--;
            break;
        case RIGHT:
            robot.r1++;
            break;
    }

    // place robot in new position
    grid[robot.r1][robot.r2].hasRobot = 1;

    // change triangle vertices
    float triangleBaseX1 = whiteRectX + robot.r1 * squareSize;
    float triangleBaseX2 = triangleBaseX1 + squareSize;
    float triangleBaseY = whiteRectY + robot.r2 * squareSize;

    switch (robot.triangleOrientation) {
        case DOWN:
            robot.triangleX[0] = (int)triangleBaseX1;
            robot.triangleY[0] = (int)triangleBaseY;
            robot.triangleX[1] = (int)triangleBaseX2;
            robot.triangleY[1] = (int)triangleBaseY;
            robot.triangleX[2] = (int)(triangleBaseX1 + (squareSize / 2));
            robot.triangleY[2] = (int)(triangleBaseY + squareSize);
            break;
        case UP:
            robot.triangleX[0] = (int)triangleBaseX1;
            robot.triangleY[0] = (int)(triangleBaseY + squareSize);
            robot.triangleX[1] = (int)triangleBaseX2;
            robot.triangleY[1] = (int)(triangleBaseY + squareSize);
            robot.triangleX[2] = (int)(triangleBaseX1 + (squareSize / 2));
            robot.triangleY[2] = (int)triangleBaseY;
            break;
        case LEFT:
            robot.triangleX[0] = (int)triangleBaseX2;
            robot.triangleY[0] = (int)triangleBaseY;
            robot.triangleX[1] = (int)triangleBaseX2;
            robot.triangleY[1] = (int)(triangleBaseY + squareSize);
            robot.triangleX[2] = (int)triangleBaseX1;
            robot.triangleY[2] = (int)(triangleBaseY + (squareSize / 2));
            break;
        case RIGHT:
            robot.triangleX[0] = (int)triangleBaseX1;
            robot.triangleY[0] = (int)triangleBaseY;
            robot.triangleX[1] = (int)triangleBaseX1;
            robot.triangleY[1] = (int)(triangleBaseY + squareSize);
            robot.triangleX[2] = (int)triangleBaseX2;
            robot.triangleY[2] = (int)(triangleBaseY + (squareSize / 2));
            break;
    }

    // redraw robot
    foreground();
    setColour(green);
    fillPolygon(3, robot.triangleX, robot.triangleY);
}

void left() {
    // rotate triangle counterclockwise
    switch (robot.triangleOrientation) {
        case UP: robot.triangleOrientation = LEFT; break;
        case DOWN: robot.triangleOrientation = RIGHT; break;
        case LEFT: robot.triangleOrientation = DOWN; break;
        case RIGHT: robot.triangleOrientation = UP; break;
    }

    float triangleBaseX = whiteRectX + robot.r1 * squareSize;
    float triangleBaseY = whiteRectY + robot.r2 * squareSize;

    switch (robot.triangleOrientation) {
        case UP:
            robot.triangleX[0] = (int)triangleBaseX;
            robot.triangleY[0] = (int)(triangleBaseY + squareSize);
            robot.triangleX[1] = (int)(triangleBaseX + squareSize);
            robot.triangleY[1] = (int)(triangleBaseY + squareSize);
            robot.triangleX[2] = (int)(triangleBaseX + (squareSize / 2));
            robot.triangleY[2] = (int)triangleBaseY;
            break;
        case LEFT:
            robot.triangleX[0] = (int)(triangleBaseX + squareSize);
            robot.triangleY[0] = (int)triangleBaseY;
            robot.triangleX[1] = (int)(triangleBaseX + squareSize);
            robot.triangleY[1] = (int)(triangleBaseY + squareSize);
            robot.triangleX[2] = (int)triangleBaseX;
            robot.triangleY[2] = (int)(triangleBaseY + (squareSize / 2));
            break;
        case DOWN:
            robot.triangleX[0] = (int)triangleBaseX;
            robot.triangleY[0] = (int)triangleBaseY;
            robot.triangleX[1] = (int)(triangleBaseX + squareSize);
            robot.triangleY[1] = (int)triangleBaseY;
            robot.triangleX[2] = (int)(triangleBaseX + (squareSize / 2));
            robot.triangleY[2] = (int)(triangleBaseY + squareSize);
            break;
        case RIGHT:
            robot.triangleX[0] = (int)triangleBaseX;
            robot.triangleY[0] = (int)triangleBaseY;
            robot.triangleX[1] = (int)triangleBaseX;
            robot.triangleY[1] = (int)(triangleBaseY + squareSize);
            robot.triangleX[2] = (int)(triangleBaseX + squareSize);
            robot.triangleY[2] = (int)(triangleBaseY + (squareSize / 2));
            break;
    }

    foreground();
    setColour(green);
    fillPolygon(3, robot.triangleX, robot.triangleY);
}

void right() {
    // rotate triangle clockwise
    switch (robot.triangleOrientation) {
        case UP: robot.triangleOrientation = RIGHT; break;
        case DOWN: robot.triangleOrientation = LEFT; break;
        case LEFT: robot.triangleOrientation = UP; break;
        case RIGHT: robot.triangleOrientation = DOWN; break;
    }

    float triangleBaseX = whiteRectX + robot.r1 * squareSize;
    float triangleBaseY = whiteRectY + robot.r2 * squareSize;

    switch (robot.triangleOrientation) {
        case UP:
            robot.triangleX[0] = (int)triangleBaseX;
            robot.triangleY[0] = (int)(triangleBaseY + squareSize);
            robot.triangleX[1] = (int)(triangleBaseX + squareSize);
            robot.triangleY[1] = (int)(triangleBaseY + squareSize);
            robot.triangleX[2] = (int)(triangleBaseX + (squareSize / 2));
            robot.triangleY[2] = (int)triangleBaseY;
            break;
        case LEFT:
            robot.triangleX[0] = (int)(triangleBaseX + squareSize);
            robot.triangleY[0] = (int)triangleBaseY;
            robot.triangleX[1] = (int)(triangleBaseX + squareSize);
            robot.triangleY[1] = (int)(triangleBaseY + squareSize);
            robot.triangleX[2] = (int)triangleBaseX;
            robot.triangleY[2] = (int)(triangleBaseY + (squareSize / 2));
            break;
        case DOWN:
            robot.triangleX[0] = (int)triangleBaseX;
            robot.triangleY[0] = (int)triangleBaseY;
            robot.triangleX[1] = (int)(triangleBaseX + squareSize);
            robot.triangleY[1] = (int)triangleBaseY;
            robot.triangleX[2] = (int)(triangleBaseX + (squareSize / 2));
            robot.triangleY[2] = (int)(triangleBaseY + squareSize);
            break;
        case RIGHT:
            robot.triangleX[0] = (int)triangleBaseX;
            robot.triangleY[0] = (int)triangleBaseY;
            robot.triangleX[1] = (int)triangleBaseX;
            robot.triangleY[1] = (int)(triangleBaseY + squareSize);
            robot.triangleX[2] = (int)(triangleBaseX + squareSize);
            robot.triangleY[2] = (int)(triangleBaseY + (squareSize / 2));
            break;
    }

    foreground();
    setColour(green);
    fillPolygon(3, robot.triangleX, robot.triangleY);
}

int atMarker() {
    if (grid[robot.r1][robot.r2].cellType == MARKER) {
        return 1;
    } else {
        return 0;
    }
}

void pickUpMarker() {
    if (grid[robot.r1][robot.r2].cellType == MARKER) {
        // remove marker from grid
        grid[robot.r1][robot.r2].cellType = EMPTY; 
        robot.markerCount++;              
        background();
        setColour(white);
        fillRect((int)(whiteRectX + robot.r1 * squareSize), (int)(whiteRectY + robot.r2 * squareSize), (int)squareSize, (int)squareSize);
        setColour(black);
        drawRect((int)(whiteRectX + robot.r1 * squareSize), (int)(whiteRectY + robot.r2 * squareSize), (int)squareSize, (int)squareSize);
        setColour(green);
        foreground();
        fillPolygon(3, robot.triangleX, robot.triangleY);
    }
}

void dropMarker() {
    // drop all markers at current position
    if (robot.markerCount > 0) {
        grid[robot.r1][robot.r2].cellType = MARKER;
        float markerX = whiteRectX + robot.r1 * squareSize;
        float markerY = whiteRectY + robot.r2 * squareSize;
        background();
        setColour(gray);
        fillRect((int)markerX, (int)markerY, (int)squareSize, (int)squareSize);
        foreground();
        setColour(green);
        fillPolygon(3, robot.triangleX, robot.triangleY);
        // reset marker count after dropping
        robot.markerCount = 0; 
    }
}

int countMarker() {
    return robot.markerCount;
}

int isAtHome() {
    if (grid[robot.r1][robot.r2].cellType == HOME) {
        return 1;
    } else {
        return 0;
    }
}

void getRobotPosition(int *x, int *y) {
    *x = robot.r1;
    *y = robot.r2;
}

int getRobotOrientation() {
    return robot.triangleOrientation;
}
