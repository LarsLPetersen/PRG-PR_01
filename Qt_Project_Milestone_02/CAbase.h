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
    {
        resetWorldSize(Nx, Ny, 1);
    }

    CAbase(int nx, int ny) :
        Ny(ny),
        Nx(nx),
        nochanges(false)
    {
        resetWorldSize(Nx, Ny, 1);
    }

    void resetWorldSize(int nx, int ny, bool del = 0);

    int getNy() {
        return Ny;
    }

    int getNx() {
        return Nx;
    }

    int getColor(int x, int y) {
        return worldColor[y*(Nx+2)+x];
    }

    void setColor(int x, int y, int c);
    void setColorEvo(int x, int y, int c);

    int getLife(int x, int y) {
        return worldLife[y*(Nx+2)+x];
    }

    void worldEvolutionLife();
    void setLife(int x, int y, int l);
    void setLifeEvo(int x, int y, int l);
    void setAlive(int x, int y, int i);
    void setAliveEvo(int x, int y, int i);

    int isAlive(int x, int y) {
        return world[y*(Nx+2)+x];
    }

    bool isNotChanged() {
        return nochanges;
    }

    int cellEvolutionLife(int x, int y);

    ~CAbase() {
    }

private:
    int Ny;
    int Nx;
    int *world;
    int *worldNew;
    int *worldColor;
    int *worldColorNew;
    int *worldLife;
    int *worldLifeNew;
    bool nochanges;
};


inline void CAbase::setAlive(int x, int y, int i) {
    // Set number i into cell with coordinates x,y in current universe
    world[y * (Nx + 2) + x] = i;
}


inline void CAbase::setAliveEvo(int x, int y, int i) {
    // set number i into cell with coordinates x,y in evolution universe
    worldNew[y * (Nx + 2) + x] = i;
}


inline void CAbase::setColor(int x, int y, int c) {
    // set color c into cell with coordinates x,y in current color universe
    worldColor[y * (Nx + 2) + x] = c;
}


inline void CAbase::setColorEvo(int x, int y, int c) {
    // set color c into cell with coordinates x,y in evolution color universe
    worldColorNew[y * (Nx + 2) + x] = c;
}


inline void CAbase::setLife(int x, int y, int l) {
    // set lifetime l into cell with coordinates x,y in current color universe
    worldLife[y * (Nx + 2) + x] = l;
}


inline void CAbase::setLifeEvo( int x, int y, int l) {
    // set lifetime l into cell with coordinates x,y in evolution color universe
    worldLifeNew[y * (Nx + 2) + x] = l;
}



// Game "Life" rules:
//
// Any live cell with fewer than two live neighbours dies, as if caused by underpopulation.
// Any live cell with two or three live neighbours lives on to the next generation.
// Any live cell with more than three live neighbours dies, as if by overpopulation.
// Any dead cell with exactly three live neighbours becomes a live cell, as if by reproduction.


inline int CAbase::cellEvolutionLife( int x, int y ) {
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
            if (isAlive(x1, y1) == 1) n_sum++;
        }
    }

    if (isAlive(x, y) == 1) {
        if(n_sum == 2 || n_sum == 3) setAliveEvo(x, y, 1);
        else setAliveEvo(x, y, 0);
    }
    else {
        if (n_sum == 3) setAliveEvo(x, y, 1);
    }
    return 0;
}


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

        // Life
        delete[] worldLife;
        delete[] worldLifeNew;
    }

    world = new int[(Ny + 2) * (Nx + 2) + 1];
    worldNew = new int[(Ny + 2) * (Nx + 2) + 1];

    // Color
    worldColor = new int[(Ny + 2) * (Nx + 2) + 1];
    worldColorNew = new int[(Ny + 2) * (Nx + 2) + 1];

    // Life
    worldLife = new int[(Ny + 2) * (Nx + 2) + 1];
    worldLifeNew = new int[(Ny + 2) * (Nx + 2) + 1];

    for (int i = 0; i <= (Ny + 2) * (Nx + 2); i++) {
        if ( (i < (Nx + 2)) || (i >= (Ny + 1) * (Nx + 2)) || (i % (Nx + 2) == 0) || (i % (Nx + 2) == (Nx + 1)) ) {
            world[i] = -1;
            worldNew[i] = -1;

            // Color
            worldColor[i] = -1;
            worldColorNew[i] = -1;

            // Life
            worldLife[i] = -1;
            worldLifeNew[i] = -1;
        }
        else {
            world[i] = 0;
            worldNew[i] = 0;

            // Color
            worldColor[i] = 0;
            worldColorNew[i] = 0;

            // Life
            worldLife[i] = 0;
            worldLifeNew[i] = 0;
        }
    }
}


inline void CAbase::worldEvolutionLife() {
    // universe evolution for every cell
    for (int ix = 1; ix <= Nx; ix++) {
        for (int iy = 1; iy <= Ny; iy++) {
                cellEvolutionLife(ix, iy);
        }
    }

    nochanges = true;

    for (int ix = 1; ix <= Nx; ix++) {           // Copy new state to current universe
        for (int iy = 1; iy <= Ny; iy++) {
            if (world[iy * (Nx + 2) + ix] != worldNew[iy * (Nx + 2) + ix]) {
                nochanges = false;
            }
            world[iy * (Nx + 2) + ix] = worldNew[iy * (Nx + 2) + ix];
        }
    }                                 // if nochanges == true, there are no evolution and we reached final stare of universe
}


#endif // CABASE_H
