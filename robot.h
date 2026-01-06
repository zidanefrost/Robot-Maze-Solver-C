#ifndef ROBOT_H
#define ROBOT_H

// orientation constants
#define DOWN 1
#define UP 0
#define LEFT 2
#define RIGHT 3

// prototypes for functions
void initializeRobot(int startX, int startY);
void forward();
void left();
void right();
void pickUpMarker();
int atMarker();
int countMarker();
void dropMarker();
int canMoveForward();
int isAtHome();
void getRobotPosition(int *x, int *y);
int getRobotOrientation();

#endif
