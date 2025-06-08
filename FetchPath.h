#ifndef FETCHPATH_H
#define FETCHPATH_H

#include "heap.h"
#include <stdint.h>
#include "DungeonGen.h"

typedef struct path_cell
{
    heap_node_t *hn;
    uint8_t pos[2];
    uint8_t from[2];
    int32_t cost;
} path_cell;

typedef enum dim
{
    dim_x,
    dim_y,
    num_dims
} dim_t;

void assignCosts(const Dungeon *d, path_cell (*path)[DUNGEON_W], int isTunnler);
void printCosts(path_cell (*path)[DUNGEON_W]);

#endif