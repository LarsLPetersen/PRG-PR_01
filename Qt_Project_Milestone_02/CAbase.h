#ifndef CABASE_H
#define CABASE_H

#include <stdlib.h>
#include <ctime>


class CAbase {

public:
    CAbase() :
        Ny(10),
        Nx(10),
        nochanges(false)
        { resetWorldSize(Nx, Ny, 1); }

    CAbase(int nx, int ny) :
        Ny(ny),
        Nx(nx),
        nochanges(false)
        { resetWorldSize(Nx, Ny, 1); }

    ~CAbase() {
    }

    int getNy() {
        return Ny;
    }

    int getNx() {
        return Nx;
    }

    int getColor(int x, int y) {
        // get color of cell x, y
        return worldColor[y * (Nx + 2) + x];
    }

    void setColor(int x, int y, int c) {
        // set color c into cell x, y in current color universe
        worldColor[y * (Nx + 2) + x] = c;
    }

    void setColorNew(int x, int y, int c){
        // set color c into cell with coordinates x,y in evolution color universe
        worldColorNew[y * (Nx + 2) + x] = c;
    }

    int getLifetime(int x, int y) {
        // get lifetime of cell x, y
        return worldLifetime[y * (Nx + 2) + x];
    }

    void setLifetime(int x, int y, int l) {
        // set lifetime l into cell with coordinates x,y in current lifetime universe
        worldLifetime[y * (Nx + 2) + x] = l;
    }

    void setLifetimeNew(int x, int y, int l) {
        // set new lifetime l for x,y
        worldLifetimeNew[y * (Nx + 2) + x] = l;
    }

    int getValue(int x, int y) {
        return world[y * (Nx + 2) + x];
    }

    void setValue(int x, int y, int i) {
        // set number i into cell with coordinates x,y in current universe
        world[y * (Nx + 2) + x] = i;
    }

    void setValueNew(int x, int y, int i) {
        // set number i into cell with coordinates x,y in evolution universe
        worldNew[y * (Nx + 2) + x] = i;
    }

    bool isNotChanged() {
        return nochanges;
    }

    void resetWorldSize(int nx, int ny, bool del = 0);

    //
    // game of life
    //
    int cellEvolutionLife(int x, int y);

    void worldEvolutionLife();

    //
    // snake
    //
    const int snakeDirections[4] = {2,    // down
                                    4,    // left
                                    6,    // right
                                    8};   // up

    const int snakeActions[3] = {0,     // move
                                 1,     // move and feed
                                 2};    // move and die

    int snakeDirection = 8;     // initial directions
    int snakeLength = 3;    //  initial length
    int snakeAction = 0;    // initial action

    int positionHead;
    int positionTail;
    int positionFood;

    int getSnakeAction();

    void setSnakeAction(int i);

    int cellEvolutionSnake(int x, int y);

    void worldEvolutionSnake();



private:
    int Ny;
    int Nx;
    int *world;
    int *worldNew;
    int *worldColor;
    int *worldColorNew;
    int *worldLifetime;
    int *worldLifetimeNew;
    bool nochanges;
};


inline void CAbase::resetWorldSize(int nx, int ny, bool del) {
    // creation or re-creation new current and evolution universe with default values (0 for non-border cell and -1 for border cell)
    Nx = nx;
    Ny = ny;

    if (!del) {
        delete[] world;
        delete[] worldNew;

        // Color
        delete[] worldColor;
        delete[] worldColorNew;

        // Lifetime
        delete[] worldLifetime;
        delete[] worldLifetimeNew;
    }

    world = new int[(Ny + 2) * (Nx + 2) + 1];
    worldNew = new int[(Ny + 2) * (Nx + 2) + 1];

    // Color
    worldColor = new int[(Ny + 2) * (Nx + 2) + 1];
    worldColorNew = new int[(Ny + 2) * (Nx + 2) + 1];

    // Lifetime
    worldLifetime = new int[(Ny + 2) * (Nx + 2) + 1];
    worldLifetimeNew = new int[(Ny + 2) * (Nx + 2) + 1];

    for (int i = 0; i <= (Ny + 2) * (Nx + 2); i++) {
        // set border cells to -1
        if ( (i < (Nx + 2)) || (i >= (Ny + 1) * (Nx + 2)) || (i % (Nx + 2) == 0) || (i % (Nx + 2) == (Nx + 1)) ) {
            world[i] = -1;
            worldNew[i] = -1;

            // Color
            worldColor[i] = -1;
            worldColorNew[i] = -1;

            // Lifetime
            worldLifetime[i] = -1;
            worldLifetimeNew[i] = -1;
        }
        else {
            world[i] = 0;
            worldNew[i] = 0;

            // Color
            worldColor[i] = 0;
            worldColorNew[i] = 0;

            // Lifetime
            worldLifetime[i] = 0;
            worldLifetimeNew[i] = 0;
        }
    }
}


// Game "Life" rules:
//
// Any live cell with fewer than two live neighbours dies, as if caused by underpopulation.
// Any live cell with two or three live neighbours lives on to the next generation.
// Any live cell with more than three live neighbours dies, as if by overpopulation.
// Any dead cell with exactly three live neighbours becomes a live cell, as if by reproduction.

inline int CAbase::cellEvolutionLife(int x, int y) {
    // Classic Game of Life. Evolution rules for every cell. Changing only cell (x, y) for every step


    int n_sum = 0;
    int x1(0), y1(0);

    for (int ix = -1; ix <= 1; ix++) {
        for (int iy = -1; iy <= 1; iy++) {
            if (ix == 0 && iy == 0) continue;
            x1 = x + ix;
            y1 = y + iy;

            if (x1 < 1) x1 = Nx;
            if (x1 > Nx) x1 = 1;
            if (y1 < 1) y1 = Ny;
            if (y1 > Ny) y1 = 1;
            if (getValue(x1, y1) == 1) n_sum++;
        }
    }

    if (getValue(x, y) == 1) {
        if (n_sum == 2 || n_sum == 3) setValueNew(x, y, 1);
        else setValueNew(x, y, 0);
    }
    else {
        if (n_sum == 3) setValueNew(x, y, 1);
    }
    return 0;
}


inline void CAbase::worldEvolutionLife() {
    // universe evolution for every cell
    for (int ix = 1; ix <= Nx; ix++) {
        for (int iy = 1; iy <= Ny; iy++) {
                cellEvolutionLife(ix, iy);
        }
    }

    nochanges = true;
    // Copy new state to current universe
    for (int ix = 1; ix <= Nx; ix++) {
        for (int iy = 1; iy <= Ny; iy++) {
            if (world[iy * (Nx + 2) + ix] != worldNew[iy * (Nx + 2) + ix]) {
                nochanges = false;
            }
            world[iy * (Nx + 2) + ix] = worldNew[iy * (Nx + 2) + ix];
        }
    }
    // if nochanges == true, there is no evolution and the universe remains constant
}


// Game "Snake" rules:
//
// ...

inline int CAbase::cellEvolutionSnake(int x, int y) {
    // Snake. Evolution rules for every cell. Changing only cell (x, y) for every step

    // snake is going to eat

    // snake is not going to eat
    int neighborhood[3][3];
    int x1(0), y1(0);

    for (int ix = -1; ix <= 1; ix++) {
        x1 = x + ix;
        if ( (x1 <= 0) || (x1 >= Nx + 1) ) x1 = 0;

        for (int iy = -1; iy <= 1; iy++) {
            y1 = y + iy;
            if ( (y1 <= 0) || (y1 >= Ny + 1) ) y1 = 0;
            neighborhood[ix + 1][iy + 1] = getValue(x1, y1);
        }
    }


    if (getValue(x, y) == 10) {

    } else {

    }
    switch (snakeDirection) {
    case 2:

        break;
    default:
        break;
    }
//    if (getValue(x, y) == 1) {
//        if(n_sum == 2 || n_sum == 3) setValueNew(x, y, 1);
//        else setValueNew(x, y, 0);
//    }
//    else {
//        if (n_sum == 3) setValueNew(x, y, 1);
//    }
    return 0;
}


inline void CAbase::worldEvolutionSnake() {
    int sA = getSnakeAction();

    switch (sA) {
    case 0:     // snake moves without feeding or dying
        // universe evolution for every cell
        for (int x = 1; x <= Nx; x++) {
            for (int y = 1; y <= Ny; y++) {
                    cellEvolutionSnake(x, y);
            }
        }
        nochanges = true;
        // Copy new state to current universe
        for (int x = 1; x <= Nx; x++) {
            for (int y = 1; y <= Ny; y++) {
                if (world[y * (Nx + 2) + x] != worldNew[y * (Nx + 2) + x]) {
                    nochanges = false;
                }
                world[y * (Nx + 2) + x] = worldNew[y * (Nx + 2) + x];
            }
        }
        // if nochanges == true, there is no evolution and the universe remains constant
        break;

    case 1:     // snake moves and feeds
        for (int x = 1; x <= Nx; x++) {
            for (int y = 1; y <= Ny; y++) {
                    //cellEvolutionSnakeFeed(x, y);
            }
        }
        nochanges = true;
        // Copy new state to current universe
        for (int x = 1; x <= Nx; x++) {
            for (int y = 1; y <= Ny; y++) {
                if (world[y * (Nx + 2) + x] != worldNew[y * (Nx + 2) + x]) {
                    nochanges = false;
                }
                world[y * (Nx + 2) + x] = worldNew[y * (Nx + 2) + x];
            }
        }
        // if nochanges == true, there is no evolution and the universe remains constant
        break;
    case 2:     // snake moves and dies
        nochanges = true;
        break;
    default:
        break;
    }
}

#endif // CABASE_H
