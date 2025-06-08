#ifndef UTIL_H
#define UTIL_H

#include "DungeonGen.h"

void resetDragonAndObjCount(Dungeon *d);
void findMaxObjAndDragon(Dungeon *d);
void pickUp(Dungeon *d, std::vector<object> &c, std::vector<object> &o, Point pos);
void abandonObj(Dungeon *d, std::vector<object> &c, int j);
#endif