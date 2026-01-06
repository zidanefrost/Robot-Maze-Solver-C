#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "graphics.h"
#include "arena.h"
#include "robot.h"

int main(void) {
    srand(time(NULL));

    setWindowAndGrid();

    drawGrid();
    // locate markers before returning to home tile
    int robotX, robotY;
    getRobotPosition(&robotX, &robotY);

    int path[MAX_GRID_SIZE][MAX_GRID_SIZE];

    // move to each marker
    for (int m = 0; m < markerCountTotal; m++) {
        resetVisitedGrid();

        int queueX[MAX_GRID_SIZE * MAX_GRID_SIZE];
        int queueY[MAX_GRID_SIZE * MAX_GRID_SIZE];
        int front = 0, rear = 0;

        queueX[rear] = robotX;
        queueY[rear] = robotY;
        rear++;
        visited[robotX][robotY] = 1;

        // create path array
        for (int i = 0; i < HORIZONTAL_SQUARES; i++) {
            for (int j = 0; j < VERTICAL_SQUARES; j++) {
                path[i][j] = -1;
            }
        }

        int destX = markers[m].x;
        int destY = markers[m].y;
        int found = 0;

        // bfs algorithm
        while (front < rear && !found) {
            int x = queueX[front];
            int y = queueY[front];
            front++;

            if (x == destX && y == destY) {
                found = 1;
                break;
            }

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
                        path[nx][ny] = x * MAX_GRID_SIZE + y;
                    }
                }
            }
        }

        if (found) {
            int currentX = destX;
            int currentY = destY;
            int tempPathX[MAX_GRID_SIZE * MAX_GRID_SIZE];
            int tempPathY[MAX_GRID_SIZE * MAX_GRID_SIZE];
            int pathLength = 0;

            while (!(currentX == robotX && currentY == robotY)) {
                tempPathX[pathLength] = currentX;
                tempPathY[pathLength] = currentY;
                pathLength++;

                int prev = path[currentX][currentY];
                currentX = prev / MAX_GRID_SIZE;
                currentY = prev % MAX_GRID_SIZE;
            }

            // change position along path
            for (int i = pathLength - 1; i >= 0; i--) {
                int nextX = tempPathX[i];
                int nextY = tempPathY[i];

                // get current orientation
                int orientation = getRobotOrientation();

                if (nextX > robotX) {
                    while (orientation != RIGHT) {
                        sleep(100);
                        clear();
                        right();
                        orientation = getRobotOrientation();
                    }
                } else if (nextX < robotX) {
                    while (orientation != LEFT) {
                        sleep(100);
                        clear();
                        left();
                        orientation = getRobotOrientation();
                    }
                } else if (nextY > robotY) {
                    while (orientation != DOWN) {
                        sleep(100);
                        clear();
                        right();
                        orientation = getRobotOrientation();
                    }
                } else if (nextY < robotY) {
                    while (orientation != UP) {
                        sleep(100);
                        clear();
                        left();
                        orientation = getRobotOrientation();
                    }
                }

                sleep(100);
                clear();
                forward();

                robotX = nextX;
                robotY = nextY;
            }

            pickUpMarker();
        } else {
            printf("No path to marker at (%d, %d)\n", destX, destY);
        }
    }

    // move back to home tile
    resetVisitedGrid();

    int queueX[MAX_GRID_SIZE * MAX_GRID_SIZE];
    int queueY[MAX_GRID_SIZE * MAX_GRID_SIZE];
    int front = 0, rear = 0;

    queueX[rear] = robotX;
    queueY[rear] = robotY;
    rear++;
    visited[robotX][robotY] = 1;

    for (int i = 0; i < HORIZONTAL_SQUARES; i++) {
        for (int j = 0; j < VERTICAL_SQUARES; j++) {
            path[i][j] = -1;
        }
    }

    int found = 0;

    while (front < rear && !found) {
        int x = queueX[front];
        int y = queueY[front];
        front++;

        if (x == homeX && y == homeY) {
            found = 1;
            break;
        }

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
                    path[nx][ny] = x * MAX_GRID_SIZE + y;
                }
            }
        }
    }

    if (found) {
        int currentX = homeX;
        int currentY = homeY;
        int tempPathX[MAX_GRID_SIZE * MAX_GRID_SIZE];
        int tempPathY[MAX_GRID_SIZE * MAX_GRID_SIZE];
        int pathLength = 0;

        while (!(currentX == robotX && currentY == robotY)) {
            tempPathX[pathLength] = currentX;
            tempPathY[pathLength] = currentY;
            pathLength++;

            int prev = path[currentX][currentY];
            currentX = prev / MAX_GRID_SIZE;
            currentY = prev % MAX_GRID_SIZE;
        }

        for (int i = pathLength - 1; i >= 0; i--) {
            int nextX = tempPathX[i];
            int nextY = tempPathY[i];

            int orientation = getRobotOrientation();

            if (nextX > robotX) {
                while (orientation != RIGHT) {
                    sleep(100);
                    clear();
                    right();
                    orientation = getRobotOrientation();
                }
            } else if (nextX < robotX) {
                while (orientation != LEFT) {
                    sleep(100);
                    clear();
                    left();
                    orientation = getRobotOrientation();
                }
            } else if (nextY > robotY) {
                while (orientation != DOWN) {
                    sleep(100);
                    clear();
                    right();
                    orientation = getRobotOrientation();
                }
            } else if (nextY < robotY) {
                while (orientation != UP) {
                    sleep(100);
                    clear();
                    left();
                    orientation = getRobotOrientation();
                }
            }

            sleep(100);
            clear();
            forward();

            robotX = nextX;
            robotY = nextY;
        }

        // confirm if robot is at home
        if (isAtHome()) {
            // drop markers at home
            dropMarker();
        } else {
            // in case of fatal error, unlikely to happen as arena is always generated using bfs
            printf("Error: Robot did not reach the home tile.\n");
        }
    } else {
        printf("No path to home tile found.\n");
    }

    return 0;
}
