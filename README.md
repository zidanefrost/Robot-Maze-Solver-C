# Robot Maze Solver

A robot maze-solving program written in C that navigates a randomly generated arena to collect markers and return them to a home tile. The simulation is graphically represented using a Java-based drawing application.

## Overview
The program generates a unique, irregularly shaped arena for every execution. The robot (represented by a green triangle) must:
1.  Identify all markers (grey tiles) within the grid.
2.  Calculate the shortest path to each marker using a Breadth-First Search (BFS) algorithm.
3.  Navigate around obstacles (black tiles) and walls (red boundaries).
4.  Pick up all markers and safely deliver them to the home tile (blue tile).

## Project Structure
* `main.c`: Coordinates the high-level logic for pathfinding and movement.
* `robot.c` / `robot.h`: Implements core robot actions like `forward()`, `left()`, `right()`, and marker interactions.
* `arena.c` / `arena.h`: Manages random grid generation, obstacle placement, and the rendering of the environment.
* `graphics.h` / `graphics.c`: Interface for the Java `drawapp-4.0.jar` visualization tool.

## Features
* **Dynamic Grid Generation**: Arenas vary in size from 10x10 to 15x15 tiles.
* **Pathfinding**: Implements BFS to ensure the robot finds the most efficient route to its targets.
* **Layered Rendering**: Uses foreground and background layers to animate the robot without redrawing the entire static arena.
* **Resilient Navigation**: Designed to avoid getting stuck in infinite loops even with complex obstacle placement.

## Getting Started

### Prerequisites
* **C Compiler**: GCC or Clang.
* **Java Runtime**: Required to run `drawapp-4.0.jar` for the graphical display.

### Compilation
To compile the program, run the following command in your terminal:
```bash
gcc -o maze_solver main.c arena.c robot.c graphics.c
```

### Execution
Execution
Run the compiled program and pipe the output to the Java drawing application:
```bash
./maze_solver | java -jar drawapp-4.0.jar
```

## Acknowledgements
Developed as part of the COMP0002 Principles of Programming coursework at UCL.
Pathfinding skeleton code assisted by ChatGPT and subsequently modified/refined independently.
