#include <stdlib.h>
#include <stdio.h>
#include "arena.h"
#include "robot.h"
#include "graphics.h"

int windowWidth, windowHeight;
float whiteRectX, whiteRectY, whiteRectWidth, whiteRectHeight;
float squareSize;

// representing grid
Cell grid[MAX_GRID_SIZE][MAX_GRID_SIZE];

// dimensions
int HORIZONTAL_SQUARES, VERTICAL_SQUARES;

// home tile coordinates
int homeX, homeY;

// marker positions
Position markers[MAX_GRID_SIZE * MAX_GRID_SIZE];
int markerCountTotal;

// visited squares for bfs
int visited[MAX_GRID_SIZE][MAX_GRID_SIZE];

void setWindowAndGrid() {
    // create random grid dimensions
    HORIZONTAL_SQUARES = MIN_SQUARES + rand() % (MAX_SQUARES - MIN_SQUARES + 1);
    VERTICAL_SQUARES = MIN_SQUARES + rand() % (MAX_SQUARES - MIN_SQUARES + 1);

    int maxSquareWidth = (700) / HORIZONTAL_SQUARES;
    int maxSquareHeight = (400) / VERTICAL_SQUARES;
    squareSize = (maxSquareWidth < maxSquareHeight) ? maxSquareWidth : maxSquareHeight;

    whiteRectWidth = HORIZONTAL_SQUARES * squareSize;
    whiteRectHeight = VERTICAL_SQUARES * squareSize;

    windowWidth = whiteRectWidth + 100;
    windowHeight = whiteRectHeight + 100;

    setWindowSize(windowWidth, windowHeight);

    whiteRectX = 50;
    whiteRectY = 50;
}

void resetVisitedGrid() {
    for (int i = 0; i < HORIZONTAL_SQUARES; i++) {
        for (int j = 0; j < VERTICAL_SQUARES; j++) {
            visited[i][j] = 0;
        }
    }
}

void generateArena() {
    // set all cells to "WALL"
    for (int i = 0; i < HORIZONTAL_SQUARES; i++) {
        for (int j = 0; j < VERTICAL_SQUARES; j++) {
            grid[i][j].cellType = WALL;
            grid[i][j].hasRobot = 0;
        }
    }

    // randomly generated arena using walk
    int totalCells = HORIZONTAL_SQUARES * VERTICAL_SQUARES;
    int accessibleCellsTarget = totalCells / 2 + rand() % (totalCells / 4); // between 1/2 to 3/4 of all cells

    // starting point
    int x = rand() % HORIZONTAL_SQUARES;
    int y = rand() % VERTICAL_SQUARES;

    grid[x][y].cellType = EMPTY; // starting cell is accessible

    int accessibleCellsCount = 1;

    while (accessibleCellsCount < accessibleCellsTarget) {
        // randomly choose direction
        int direction = rand() % 4;
        int nx = x;
        int ny = y;

        switch (direction) {
            case 0: ny = y - 1; break; // up
            case 1: nx = x + 1; break; // right
            case 2: ny = y + 1; break; // down
            case 3: nx = x - 1; break; // left
        }

        // check for bounds
        if (nx >= 0 && nx < HORIZONTAL_SQUARES && ny >= 0 && ny < VERTICAL_SQUARES) {
            if (grid[nx][ny].cellType == WALL) {
                grid[nx][ny].cellType = EMPTY;
                accessibleCellsCount++;
            }
            x = nx;
            y = ny;
        } else {
            // pick new accessible cell
            do {
                x = rand() % HORIZONTAL_SQUARES;
                y = rand() % VERTICAL_SQUARES;
            } while (grid[x][y].cellType != EMPTY);
        }
    }

    // robot must not start at a marker, wall or obstacle
    int robotX, robotY;
    do {
        robotX = rand() % HORIZONTAL_SQUARES;
        robotY = rand() % VERTICAL_SQUARES;
    } while (grid[robotX][robotY].cellType != EMPTY);

    // robot staring pos
    grid[robotX][robotY].hasRobot = 1;


    initializeRobot(robotX, robotY);

    // bfs to find accessible cells
    int accessibleCells[MAX_GRID_SIZE * MAX_GRID_SIZE][2];
    int accessibleCellsNum = 0;

    // visited grid initialization
    resetVisitedGrid();

    int queueX[MAX_GRID_SIZE * MAX_GRID_SIZE];
    int queueY[MAX_GRID_SIZE * MAX_GRID_SIZE];
    int front = 0, rear = 0;

    queueX[rear] = robotX;
    queueY[rear] = robotY;
    rear++;
    visited[robotX][robotY] = 1;
    accessibleCells[accessibleCellsNum][0] = robotX;
    accessibleCells[accessibleCellsNum][1] = robotY;
    accessibleCellsNum++;

    while (front < rear) {
        int x = queueX[front];
        int y = queueY[front];
        front++;

        int dx[] = {0, 1, 0, -1};
        int dy[] = {-1, 0, 1, 0};

        for (int i = 0; i < 4; i++) {
            int nx = x + dx[i];
            int ny = y + dy[i];

            if (nx >= 0 && nx < HORIZONTAL_SQUARES && ny >= 0 && ny < VERTICAL_SQUARES) {
                if (!visited[nx][ny] && grid[nx][ny].cellType != WALL && grid[nx][ny].cellType != OBSTACLE) {
                    queueX[rear] = nx;
                    queueY[rear] = ny;
                    rear++;
                    visited[nx][ny] = 1;
                    accessibleCells[accessibleCellsNum][0] = nx;
                    accessibleCells[accessibleCellsNum][1] = ny;
                    accessibleCellsNum++;
                }
            }
        }
    }

    // random placement of home tile
    do {
        int idx = rand() % accessibleCellsNum;
        homeX = accessibleCells[idx][0];
        homeY = accessibleCells[idx][1];
    } while (grid[homeX][homeY].cellType != EMPTY || (homeX == robotX && homeY == robotY));
    grid[homeX][homeY].cellType = HOME;

    // place markers
    markerCountTotal = 0;

    int maxMarkers = accessibleCellsNum / 5;
    int numMarkers = rand() % maxMarkers + 1;

    for (int i = 0; i < numMarkers; i++) {
        int markerX, markerY;
        do {
            int idx = rand() % accessibleCellsNum;
            markerX = accessibleCells[idx][0];
            markerY = accessibleCells[idx][1];
        } while (grid[markerX][markerY].cellType != EMPTY || (markerX == robotX && markerY == robotY) || (markerX == homeX && markerY == homeY));
        grid[markerX][markerY].cellType = MARKER;
        // set marker pos
        markers[markerCountTotal].x = markerX;
        markers[markerCountTotal].y = markerY;
        markerCountTotal++;
    }

    // place obstacles
    int maxObstacles = accessibleCellsNum / 5;
    int numObstacles = rand() % maxObstacles + 1;

    int attemptsPerObstacle = 10;

    for (int i = 0; i < numObstacles; i++) {
        int obstaclePlaced = 0;
        for (int attempt = 0; attempt < attemptsPerObstacle && !obstaclePlaced; attempt++) {
            int obsX, obsY;
            int idx = rand() % accessibleCellsNum;
            obsX = accessibleCells[idx][0];
            obsY = accessibleCells[idx][1];

            if (grid[obsX][obsY].cellType != EMPTY || (obsX == robotX && obsY == robotY) || (obsX == homeX && obsY == homeY))
                continue;

            int isMarker = 0;
            for (int m = 0; m < markerCountTotal; m++) {
                if (obsX == markers[m].x && obsY == markers[m].y) {
                    isMarker = 1;
                    break;
                }
            }
            if (isMarker)
                continue;

            grid[obsX][obsY].cellType = OBSTACLE;

            resetVisitedGrid();
            int queueX[MAX_GRID_SIZE * MAX_GRID_SIZE];
            int queueY[MAX_GRID_SIZE * MAX_GRID_SIZE];
            int front = 0, rear = 0;

            queueX[rear] = robotX;
            queueY[rear] = robotY;
            rear++;
            visited[robotX][robotY] = 1;

            int homeReachable = 0;
            int markersVisited = 0;

            while (front < rear) {
                int x = queueX[front];
                int y = queueY[front];
                front++;

                if (x == homeX && y == homeY)
                    homeReachable = 1;

                for (int m = 0; m < markerCountTotal; m++) {
                    if (x == markers[m].x && y == markers[m].y)
                        markersVisited++;
                }

                if (homeReachable && markersVisited == markerCountTotal)
                    break;

                int dx[] = {0, 1, 0, -1};
                int dy[] = {-1, 0, 1, 0};

                for (int k = 0; k < 4; k++) {
                    int nx = x + dx[k];
                    int ny = y + dy[k];

                    if (nx >= 0 && nx < HORIZONTAL_SQUARES && ny >= 0 && ny < VERTICAL_SQUARES) {
                        if (!visited[nx][ny] && grid[nx][ny].cellType != WALL && grid[nx][ny].cellType != OBSTACLE) {
                            queueX[rear] = nx;
                            queueY[rear] = ny;
                            rear++;
                            visited[nx][ny] = 1;
                        }
                    }
                }
            }

            if (homeReachable && markersVisited == markerCountTotal) {
                obstaclePlaced = 1;
            } else {
                grid[obsX][obsY].cellType = EMPTY;
            }
        }
    }
}

void drawGrid() {
    generateArena();

    // window filled with red as background
    background();
    setColour(red);
    fillRect(0, 0, windowWidth, windowHeight);

    // grid is drawn
    for (int i = 0; i < HORIZONTAL_SQUARES; i++) {
        for (int j = 0; j < VERTICAL_SQUARES; j++) {
            int x = (int)(whiteRectX + i * squareSize);
            int y = (int)(whiteRectY + j * squareSize);

            switch (grid[i][j].cellType) {
                case WALL:
                    continue;
                case EMPTY:
                    setColour(white);
                    break;
                case OBSTACLE:
                    setColour(black);
                    break;
                case MARKER:
                    setColour(gray);
                    break;
                case HOME:
                    setColour(blue);
                    break;
            }
            fillRect(x, y, (int)squareSize, (int)squareSize);
            setColour(black);
            drawRect(x, y, (int)squareSize, (int)squareSize);
        }
    }

    int robotX, robotY;
    getRobotPosition(&robotX, &robotY);

    // calculate triangle vertices
    int triangleBaseX1 = (int)(whiteRectX + robotX * squareSize);
    int triangleBaseX2 = (int)(triangleBaseX1 + squareSize);
    int triangleBaseY = (int)(whiteRectY + robotY * squareSize);

    int orientation = getRobotOrientation();
    int robotTriangleX[3], robotTriangleY[3];

    switch (orientation) {
        case DOWN:
            robotTriangleX[0] = triangleBaseX1;
            robotTriangleY[0] = triangleBaseY;
            robotTriangleX[1] = triangleBaseX2;
            robotTriangleY[1] = triangleBaseY;
            robotTriangleX[2] = triangleBaseX1 + (int)(squareSize / 2);
            robotTriangleY[2] = triangleBaseY + (int)squareSize;
            break;
        case UP:
            robotTriangleX[0] = triangleBaseX1;
            robotTriangleY[0] = triangleBaseY + (int)squareSize;
            robotTriangleX[1] = triangleBaseX2;
            robotTriangleY[1] = triangleBaseY + (int)squareSize;
            robotTriangleX[2] = triangleBaseX1 + (int)(squareSize / 2);
            robotTriangleY[2] = triangleBaseY;
            break;
        case LEFT:
            robotTriangleX[0] = triangleBaseX2;
            robotTriangleY[0] = triangleBaseY;
            robotTriangleX[1] = triangleBaseX2;
            robotTriangleY[1] = triangleBaseY + (int)squareSize;
            robotTriangleX[2] = triangleBaseX1;
            robotTriangleY[2] = triangleBaseY + (int)(squareSize / 2);
            break;
        case RIGHT:
            robotTriangleX[0] = triangleBaseX1;
            robotTriangleY[0] = triangleBaseY;
            robotTriangleX[1] = triangleBaseX1;
            robotTriangleY[1] = triangleBaseY + (int)squareSize;
            robotTriangleX[2] = triangleBaseX2;
            robotTriangleY[2] = triangleBaseY + (int)(squareSize / 2);
            break;
    }

    foreground();
    fillPolygon(3, robotTriangleX, robotTriangleY);
}
