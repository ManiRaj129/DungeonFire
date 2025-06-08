#ifndef PRINTUTIL_H
#define PRINTUTIL_H

#include "DungeonGen.h"
#include "objectTypes.h"
// inclusive bounds
#define rand_range(min, max) ((rand() % (((max) + 1) - (min))) + (min))

#define isInBound(x, y) (x < (DUNGEON_W - 1) && x > 0 && y < (DUNGEON_H - 1) && y > 0)
#define visibleRadius 5
#define isVisible(npcX, npcY, pcX, pcY) (npcX > (pcX - visibleRadius) && npcX < (pcX + visibleRadius) && npcY > (pcY - visibleRadius) && npcY < (pcY + visibleRadius))


void printDungeonNC(Dungeon *dungeon);
void printVisible(Dungeon *dungeon);
#endif