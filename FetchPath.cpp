
#include "FetchPath.h"

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

typedef int16_t pair_t[num_dims];

#define getType(pair) (d->grid[pair[dim_y]][pair[dim_x]].type)
#define getTypeAt(x, y) (d->grid[y][x].type)
#define getHard(pair) (d->grid[pair[dim_y]][pair[dim_x]].hardness)
#define getHardAt(x, y) (d->grid[y][x].hardness)

static int32_t path_cmp(const void *key, const void *with)
{
    return ((path_cell *)key)->cost - ((path_cell *)with)->cost;
}

/*
player co-ordinates available find and assign the shortest distance (minimum total weight of all the path)
from the cell to player cell for each of the cell in the grid.
*/
// when we pass an array pointer like this it does not carry the info about its
// size in this case we can't figure the number of rows , i.e, Dungeon Height so in general cases we need to pass argument to inform the size.

void assignCosts(const Dungeon *d, path_cell (*path)[DUNGEON_W], int isTunnler)
{
    Point player= d->PlayerPos;
    
    path_cell *p;
    heap_t h;
    uint32_t x, y;

    // intialise path
    for (y = 0; y < DUNGEON_H; y++)
    {
        for (x = 0; x < DUNGEON_W; x++)
        {
            path[y][x].pos[dim_y] = y;
            path[y][x].pos[dim_x] = x;
        }
    }

    for (y = 0; y < DUNGEON_H; y++)
    {
        for (x = 0; x < DUNGEON_W; x++)
        {
            path[y][x].cost = INT32_MAX;
        }
    }

    path[player.y][player.x].cost = 0;

    heap_init(&h, path_cmp, NULL);

    for (y = 0; y < DUNGEON_H; y++)
    {
        for (x = 0; x < DUNGEON_W; x++)
        {
            if (isTunnler)
            {
                if (getTypeAt(x, y) != immutableRock)
                {
                    path[y][x].hn = heap_insert(&h, &path[y][x]);
                }
                else
                {
                    path[y][x].hn = NULL;
                }
            }
            else
            {
                if (getTypeAt(x, y) != immutableRock && getTypeAt(x, y) != rockC)
                {
                    path[y][x].hn = heap_insert(&h, &path[y][x]);
                }
                else
                {
                    path[y][x].hn = NULL;
                }
            }
        }
    }

    while ((p = (path_cell *)heap_remove_min(&h)))
    {
        p->hn = NULL;

        // top cell
        if ((path[p->pos[dim_y] - 1][p->pos[dim_x]].hn) &&
            (path[p->pos[dim_y] - 1][p->pos[dim_x]].cost > p->cost + (1 + (getHard(p->pos) / 85))))
        {
            path[p->pos[dim_y] - 1][p->pos[dim_x]].cost = p->cost + (1 + (getHard(p->pos) / 85));

            path[p->pos[dim_y] - 1][p->pos[dim_x]].from[dim_y] = p->pos[dim_y];

            path[p->pos[dim_y] - 1][p->pos[dim_x]].from[dim_x] = p->pos[dim_x];

            heap_decrease_key_no_replace(&h, path[p->pos[dim_y] - 1][p->pos[dim_x]].hn);
        }

        // left cell
        if ((path[p->pos[dim_y]][p->pos[dim_x] - 1].hn) &&
            (path[p->pos[dim_y]][p->pos[dim_x] - 1].cost > p->cost + (1 + (getHard(p->pos) / 85))))
        {
            path[p->pos[dim_y]][p->pos[dim_x] - 1].cost = p->cost + (1 + (getHard(p->pos) / 85));

            path[p->pos[dim_y]][p->pos[dim_x] - 1].from[dim_y] = p->pos[dim_y];

            path[p->pos[dim_y]][p->pos[dim_x] - 1].from[dim_x] = p->pos[dim_x];

            heap_decrease_key_no_replace(&h, path[p->pos[dim_y]][p->pos[dim_x] - 1].hn);
        }

        // right cell
        if ((path[p->pos[dim_y]][p->pos[dim_x] + 1].hn) &&
            (path[p->pos[dim_y]][p->pos[dim_x] + 1].cost > p->cost + (1 + (getHard(p->pos) / 85))))
        {
            path[p->pos[dim_y]][p->pos[dim_x] + 1].cost = p->cost + (1 + (getHard(p->pos) / 85));

            path[p->pos[dim_y]][p->pos[dim_x] + 1].from[dim_y] = p->pos[dim_y];

            path[p->pos[dim_y]][p->pos[dim_x] + 1].from[dim_x] = p->pos[dim_x];

            heap_decrease_key_no_replace(&h, path[p->pos[dim_y]][p->pos[dim_x] + 1].hn);
        }

        // down cell
        if ((path[p->pos[dim_y] + 1][p->pos[dim_x]].hn) &&
            (path[p->pos[dim_y] + 1][p->pos[dim_x]].cost > p->cost + (1 + (getHard(p->pos) / 85))))
        {
            path[p->pos[dim_y] + 1][p->pos[dim_x]].cost = p->cost + (1 + (getHard(p->pos) / 85));

            path[p->pos[dim_y] + 1][p->pos[dim_x]].from[dim_y] = p->pos[dim_y];

            path[p->pos[dim_y] + 1][p->pos[dim_x]].from[dim_x] = p->pos[dim_x];

            heap_decrease_key_no_replace(&h, path[p->pos[dim_y] + 1][p->pos[dim_x]].hn);
        }

        // Diagonal Moments

        // top-left
        if ((path[p->pos[dim_y] - 1][p->pos[dim_x] - 1].hn) &&
            (path[p->pos[dim_y] - 1][p->pos[dim_x] - 1].cost > p->cost + (1 + (getHard(p->pos) / 85))))
        {
            path[p->pos[dim_y] - 1][p->pos[dim_x] - 1].cost = p->cost + (1 + (getHard(p->pos) / 85));

            path[p->pos[dim_y] - 1][p->pos[dim_x] - 1].from[dim_y] = p->pos[dim_y];

            path[p->pos[dim_y] - 1][p->pos[dim_x] - 1].from[dim_x] = p->pos[dim_x];

            heap_decrease_key_no_replace(&h, path[p->pos[dim_y] - 1][p->pos[dim_x] - 1].hn);
        }

        // top-right
        if ((path[p->pos[dim_y] - 1][p->pos[dim_x] + 1].hn) &&
            (path[p->pos[dim_y] - 1][p->pos[dim_x] + 1].cost > p->cost + (1 + (getHard(p->pos) / 85))))
        {
            path[p->pos[dim_y] - 1][p->pos[dim_x] + 1].cost = p->cost + (1 + (getHard(p->pos) / 85));

            path[p->pos[dim_y] - 1][p->pos[dim_x] + 1].from[dim_y] = p->pos[dim_y];

            path[p->pos[dim_y] - 1][p->pos[dim_x] + 1].from[dim_x] = p->pos[dim_x];

            heap_decrease_key_no_replace(&h, path[p->pos[dim_y] - 1][p->pos[dim_x] + 1].hn);
        }

        // down-left
        if ((path[p->pos[dim_y] + 1][p->pos[dim_x] - 1].hn) &&
            (path[p->pos[dim_y] + 1][p->pos[dim_x] - 1].cost > p->cost + (1 + (getHard(p->pos) / 85))))
        {
            path[p->pos[dim_y] + 1][p->pos[dim_x] - 1].cost = p->cost + (1 + (getHard(p->pos) / 85));

            path[p->pos[dim_y] + 1][p->pos[dim_x] - 1].from[dim_y] = p->pos[dim_y];

            path[p->pos[dim_y] + 1][p->pos[dim_x] - 1].from[dim_x] = p->pos[dim_x];

            heap_decrease_key_no_replace(&h, path[p->pos[dim_y] + 1][p->pos[dim_x] - 1].hn);
        }

        // down-right
        if ((path[p->pos[dim_y] + 1][p->pos[dim_x] + 1].hn) &&
            (path[p->pos[dim_y] + 1][p->pos[dim_x] + 1].cost > p->cost + (1 + (getHard(p->pos) / 85))))
        {
            path[p->pos[dim_y] + 1][p->pos[dim_x] + 1].cost = p->cost + (1 + (getHard(p->pos) / 85));

            path[p->pos[dim_y] + 1][p->pos[dim_x] + 1].from[dim_y] = p->pos[dim_y];

            path[p->pos[dim_y] + 1][p->pos[dim_x] + 1].from[dim_x] = p->pos[dim_x];

            heap_decrease_key_no_replace(&h, path[p->pos[dim_y] + 1][p->pos[dim_x] + 1].hn);
        }
    }
}


void printCosts(path_cell (*path)[DUNGEON_W])
{
    int y, x;

    for (y = 0; y < DUNGEON_H; y++)
    {
        for (x = 0; x < DUNGEON_W; x++)
        {
            if (path[y][x].cost == 0)
            {
                printf("@");
            }
            else if (path[y][x].cost == INT32_MAX)
            {
                printf(" ");
            }
            else
            {
                int printable = path[y][x].cost % 10;
                printf("%d", printable);
            }
        }
        printf("\n");
    }
    printf("\n");
}
