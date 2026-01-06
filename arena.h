#ifndef ARENA_H
#define ARENA_H

// grid dimension ranges
#define MIN_SQUARES 10
#define MAX_SQUARES 15

// cell types
#define WALL -1
#define EMPTY 0
#define OBSTACLE 1
#define MARKER 2
#define HOME 3

#define MAX_GRID_SIZE 20

typedef struct {
    int cellType; // WALL, EMPTY, OBSTACLE, MARKER, HOME
    int hasRobot; // 0 or 1
} Cell;

typedef struct {
    int x, y;
} Position;

// global variables
extern int windowWidth, windowHeight;
extern float whiteRectX, whiteRectY, whiteRectWidth, whiteRectHeight;
extern float squareSize;

extern Cell grid[MAX_GRID_SIZE][MAX_GRID_SIZE]; 

extern int HORIZONTAL_SQUARES, VERTICAL_SQUARES;

extern int homeX, homeY;

extern Position markers[MAX_GRID_SIZE * MAX_GRID_SIZE];
extern int markerCountTotal;

// prototypes for functions
void setWindowAndGrid();
void drawGrid();
void resetVisitedGrid();
void generateArena();

extern int visited[MAX_GRID_SIZE][MAX_GRID_SIZE];

#endif
