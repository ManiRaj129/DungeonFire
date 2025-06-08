#include <iostream>
#include <stdlib.h>
#include "DungeonGen.h"
#include "Movement.h"
#include "ncurses.h"
#include "Fog.h"

#define True 1
#define False 0


/**
 * Marks the 3x3 cells centered at the given point as visitied
 *
 * param
 * d: pointer to dungeon
 * p: newly visited cell
 */
void markVisited(Dungeon *d, Point p)
{

    int exp[] = {+2, -2, +1, -1, 0};
    int x,
        y;
    for (int i = 0; i < 5; i++)
    {
        y = p.y + exp[i];
        for (int j = 0; j < 5; j++)
        {
            x = p.x + exp[j];
            if (isInBound(x, y))
            {
                d->visitedGrid[y][x] = True;
            }
        }
    }
}
