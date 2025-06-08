#include "util.h"
#include "descriptions.h"
#include "Movement.h"



void markFoundObjDesc(Dungeon *d, std::string name)
{
    for (auto &objDesc : d->object_descriptions)
    {
        if (objDesc.get_name() == name)
        {
            objDesc.find();
            break;
        }
    }
}

void pickUp(Dungeon *d, std::vector<object> &c, std::vector<object> &o, Point pos)
{
    for (int i = 0; i < (int)o.size();)
    {
        if (o[i].loc.x == pos.x && o[i].loc.y == pos.y)
        {
            c.push_back(o[i]);
            markFoundObjDesc(d, o[i].name);
            o.erase(o.begin() + i);
            break;
        }
        else
        {

            ++i;
        }
    }
}

void abandonObj(Dungeon *d, std::vector<object> &c, int j)
{
    for (int i = 0; i < (int)d->object_descriptions.size(); i++)
    {

        if (d->object_descriptions[i].get_name() == c[j].name)
        {
            d->object_descriptions.erase(d->object_descriptions.begin() + i);
            break;
        }
    }
    c.erase(c.begin() + j);
}

void findMaxObjAndDragon(Dungeon *d)
{

    int roomCells = 0;
    for (int i = 0; i < d->roomCount; i++)
    {
        roomCells += d->rooms[i].height * d->rooms[i].width;
    }

    // atmost 80 percent of room space could be used by Dragons where 50 by objects
    d->maxDragon = (80 * roomCells) / 100;
    d->maxObj = (50 * roomCells) / 100;
}

// generated or live count is set to zero as the level change regenarates new instances
// however the killed and found status are unmodified
void resetDragonAndObjCount(Dungeon *d)
{

    for (int i = 0; i < (int)d->monster_descriptions.size(); i++)
    {
        d->monster_descriptions[i].resetCount();
    }

    for (int j = 0; j < (int)d->object_descriptions.size(); j++)
    {
        d->object_descriptions[j].resetCount();
    }
}