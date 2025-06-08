#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include "FetchPath.h"
#include "Movement.h"
#include <unistd.h>
#include <vector>
#include "objectTypes.h"

#define assign_dragnon_type ((rand_range(0, 1) * INTELLIGENT_DRAGON) | (rand_range(0, 1) * TELEPATH_DRAGON) | \
                             (rand_range(0, 1) * TUNNELING_DRAGON) | (rand_range(0, 1) * ERRATIC_DRAGON))

#define dragon_speed rand_range(5, 20)

path_cell TMap[DUNGEON_H][DUNGEON_W];
path_cell NTMap[DUNGEON_H][DUNGEON_W];

#define resetTMap assignCosts(d, TMap, 1)
#define resetNTMap assignCosts(d, NTMap, 0)

// when a occupant move to other cell make current grid cell * and for the new cell assign occupant
// first with turn,if that can't determine then again with sNo
static int32_t occupant_cmp(const void *key, const void *with)
{
    int32_t val = ((occupant_t *)key)->turn - ((occupant_t *)with)->turn;

    if (val == 0)
    {
        val = ((occupant_t *)key)->sNo - ((occupant_t *)with)->sNo;
    }
    return val;
}

// assign heap nodes, memory for dragons
void setOccupants(Dungeon *d, heap_t *h)
{

    heap_init(h, occupant_cmp, NULL);

    // sets the distance maps
    resetTMap;
    resetNTMap;

    int serialNo = 1;

    d->occupants.push_back(new player_t);
    // set player
    d->occupants[0]->sNo = serialNo++;

    d->occupants[0]->turn = 0;

    d->occupants[0]->pos = d->PlayerPos;
    d->occupants[0]->hn = heap_insert(h, d->occupants[0]);

    int i;
    int x;
    int y;
    int roomIndex;

    int j = 1;

    printf("%d", d->DragonNum);
    // allocate dragons
    while (j <= d->DragonNum)
    {
        i = rand() % (int)d->monster_descriptions.size();
        if (d->monster_descriptions[i].can_be_generated() && d->monster_descriptions[i].pass_rarity_roll())
        {

            d->occupants.push_back(new dragon_t(d->monster_descriptions[i]));

            d->occupants[j]->sNo = serialNo++;
            d->occupants[j]->turn = 0;

            // place dragon in rooms randomly
            roomIndex = rand() % d->roomCount;
            do
            {
                x = d->rooms[roomIndex].scol + (rand() % d->rooms[roomIndex].width);
                y = d->rooms[roomIndex].srow + (rand() % d->rooms[roomIndex].height);
            } while (d->grid[y][x].occupant != '*');
            d->occupants[j]->pos.x = x;
            d->occupants[j]->pos.y = y;

            d->grid[y][x].occupant = d->occupants[j]->type;
            d->occupants[j]->hn = heap_insert(h, d->occupants[j]);

            d->monster_descriptions[i].birth();

            j++;
        }
    }
}

void set_objects(Dungeon *d)
{

    int i;
    int x;
    int y;
    int roomIndex;

    int j = 0;
    // allocate OBJECTs
    while (j < d->ObjNum)
    {

        i = rand() % (int)d->object_descriptions.size();
        if (d->object_descriptions[i].can_be_generated() && d->object_descriptions[i].pass_rarity_roll())
        {
            d->objects.push_back(object(d->object_descriptions[i]));
            // place dragon in rooms randomly
            roomIndex = rand() % d->roomCount;
            do
            {
                x = d->rooms[roomIndex].scol + (rand() % d->rooms[roomIndex].width);
                y = d->rooms[roomIndex].srow + (rand() % d->rooms[roomIndex].height);
            } while (d->grid[y][x].obj != objtype_no_type);

            d->grid[y][x].obj = d->objects[j].type;

            d->objects[j].loc.x = x;
            d->objects[j].loc.y = y;

            d->object_descriptions[i].generate();
            j++;
        }
    }
}

// determines whether player is visible from dragon position, i.e., clear path between monster and player.
// return 0 if not visible, else 1.  NO boundaries and walls in the line of sight. Bresenham's algorithm.
int isPlayerVisible(Dungeon *d, Point dragPos, Point playerPos)
{
    int x1 = dragPos.x;
    int y1 = dragPos.y;
    int x2 = playerPos.x;
    int y2 = playerPos.y;

    int dx = abs(x1 - x2);
    int dy = abs(y1 - y2);

    int stepx = (x2 > x1) ? 1 : -1;
    int stepy = (y2 > y1) ? 1 : -1;

    int err = dx - dy;
    int e2;
    // no need to check the cells that dragon and player rest. however the following check the player cell as well.
    while (x1 != x2 && y1 != y2)
    {

        e2 = 2 * err;
        if (e2 > -dy)
        {
            err -= dy;
            x1 += stepx;
        }
        if (e2 < dx)
        {
            err += dx;
            y1 += stepy;
        }
        // check the cell for block (i.e., wall) where the x1 and y1 are the current cell co-ordinates
        if (d->grid[y1][x1].type == rockC)
        {
            return 0;
        }
    }
    return 1;
}

// return 1 when killed boss if not 0 when any other killed
int markDragDescKill(Dungeon *d, dragon_t *drag)
{
    for (int i = 0; i < (int)d->monster_descriptions.size(); i++)
    {
        if (drag->name == d->monster_descriptions[i].get_name())
        {
            d->monster_descriptions[i].die();
        }
    }

    if (drag->abilities & NPC_BOSS)
    {
        return 1;
    }
    return 0;
}

int calDamageForPlayer(player_t *p)
{
    int totalDamage = p->damage.roll();

    for (auto &obj : p->equipped)
    {
        totalDamage += obj.damage.roll();
    }
    return totalDamage;
}

// return 1 when killed boss if not 0 when any other killed and -1 when just attacked.
int killOccupant(Point pos, Dungeon *d, heap_t *h)
{
    int totalOcuupants = d->DragonNum + 1;
    int totalDamage;
    for (int i = 1; i < totalOcuupants; i++)
    {
        if (pos.x == d->occupants[i]->pos.x && pos.y == d->occupants[i]->pos.y)
        {
            totalDamage = calDamageForPlayer((player_t *)d->occupants[0]);

            d->occupants[i]->hitpoints -= totalDamage;
            if (d->occupants[i]->hitpoints <= 0)
            {
                if (d->occupants[i]->hn)
                {
                    d->occupants[i]->hn = NULL;
                    return markDragDescKill(d, (dragon_t *)d->occupants[i]);
                }
            }
            break;
        }
    }
    return -1;
}

void updateDragLoc(int newX, int newY, int x, int y, Dungeon *d)
{

    int totalOcuupants = d->DragonNum + 1;
    for (int i = 1; i < totalOcuupants; i++)
    {
        if (x == d->occupants[i]->pos.x && y == d->occupants[i]->pos.y)
        {
            d->occupants[i]->pos.x = newX;
            d->occupants[i]->pos.y = newY;
            break;
        }
    }
}

int attackPlayer(Dungeon *d, dragon_t *drag)
{
    d->occupants[0]->hitpoints -= drag->damage.roll();
    if (d->occupants[0]->hitpoints <= 0)
    {
        return 1;
    }
    return 0;
}
/*
Unused:  Newer version takes inputs from the user theough ncurses interface

Move to random neighbour*/
void player_move(Dungeon *d, player_t *p, heap_t *h)
{

    int x = p->pos.x;
    int y = p->pos.y;
    // making current cell unoccupied
    d->grid[y][x].occupant = '*';
    // at each move +1 or -1 for each co-ordinate ensureing within the bounds
    int tempx, tempy;
    int adjustX;
    int adjustY;
    do
    {
        adjustX = rand_range(0, 1) ? 1 : -1;
        adjustY = rand_range(0, 1) ? 1 : -1;
        tempx = x + adjustX;
        tempy = y + adjustY;
    } while (!isInBound(tempx, tempy));

    x = tempx;
    y = tempy;
    Point pos = {x, y};

    if (d->grid[y][x].occupant != '*')
    {
        killOccupant(pos, d, h);
    }

    if (d->grid[y][x].type == rockC)
    {
        d->grid[y][x].type = corridorC;
        d->grid[y][x].hardness = 0;
        resetNTMap;
        resetTMap;
    }

    d->grid[y][x].occupant = '@';
    d->PlayerPos = pos;

    p->pos = pos;
    p->turn += (1000 / p->speed);
    p->hn = heap_insert(h, p);
}

/*intelligent: short path and remember lastseen pc location,
  if not move in a straight line only one of eight directions so it may trap (if non tunneling)
  and can't remember last seen.

 telepath: always know the current pc location,if not can only know the pc when in
 line of sight as determined by bresenham's algo. (determines the pc position)

 tunneling: can tunnel the rock but when attempted only reduces the 85 hardness from
 the rock can pass through only if the rock hardness become zero or less at that attempt.
 if resulted rock hardness >0 only update the TMap for >=0 both maps should be updated.
 Non-tunneleling just passes through open space no map updates.(determines the map type)

 erratic: have 50% probability to obey other characteristics, and other cases random, if not 100% obey other characteristics.

 Note: returns 1 when dragon move killed PC if not 0
 */
int dragon_move(Dungeon *d, dragon_t *p, heap_t *h)
{
    int x = p->pos.x;
    int y = p->pos.y;
    int newX;
    int newY;

    int isNotRandom = 1;

    char dType = p->type;
    if (dType & ERRATIC_DRAGON)
    {
        // random case
        if (rand_range(0, 1))
        {
            isNotRandom = 0;
        }
    }

    Point playerCell;
    int isTunneling = 0;

    if (isNotRandom)
    {
        if ((dType & TELEPATH_DRAGON) || isPlayerVisible(d, p->pos, d->PlayerPos))
        {
            playerCell = d->PlayerPos;
        }
        else
        {
            // if ever the targetPos tracked then it means it is intelligent if it is not then the targetPos just hold default values
            playerCell = p->targetPos;
        }

        // if not default values
        if (playerCell.x != 0 && playerCell.y != 0)
        {
            if (dType & INTELLIGENT_DRAGON)
            {
                // PC location tracked
                p->targetPos = playerCell;

                // shortest path
                if (dType & TUNNELING_DRAGON)
                {
                    isTunneling = 1;
                    newX = TMap[y][x].from[dim_x];
                    newY = TMap[y][x].from[dim_y];
                }
                else
                {
                    newX = NTMap[y][x].from[dim_x];
                    newY = NTMap[y][x].from[dim_y];
                }
            }
            else
            {
                newX = x;
                newY = y;
                // straight line and no tracking of PC
                if (x < playerCell.x)
                {
                    newX += 1;
                }
                else if (x > playerCell.x)
                {
                    newX -= 1;
                }
                if (y < playerCell.y)
                {
                    newY += 1;
                }
                else if (y > playerCell.y)
                {
                    newY -= 1;
                }

                if (!isTunneling)
                {
                    if (d->grid[newY][newX].type == rockC)
                    {
                        // trapped at the current cell
                        p->turn += (1000 / p->speed);
                        p->hn = heap_insert(h, p);
                        return 0;
                    }
                }
            }
        }
        else
        {
            // no move as player pos is not known
            p->turn += (1000 / p->speed);
            p->hn = heap_insert(h, p);
            return 0;
        }
    }
    else
    {
        int adjustX;
        int adjustY;
        // random case
        if (dType & TUNNELING_DRAGON)
        {
            do
            {
                adjustX = rand_range(0, 1) ? 1 : -1;
                adjustY = rand_range(0, 1) ? 1 : -1;
                newX = x + adjustX;
                newY = y + adjustY;
            } while (!isInBound(newX, newY));
        }
        else
        {
            do
            {
                adjustX = rand_range(0, 1) ? 1 : -1;
                adjustY = rand_range(0, 1) ? 1 : -1;
                newX = x + adjustX;
                newY = y + adjustY;
            } while (!isInBound(newX, newY) && d->grid[newY][newX].type != rockC);
        }
    }

    Point pos = {newX, newY};
    int isAttacked = 0;
    int isNPC = 0;
    if (d->grid[newY][newX].occupant != '*')
    {
        if (d->grid[newY][newX].occupant == PLAYER)
        {
            if (attackPlayer(d, p))
            {
                d->grid[y][x].occupant = '*';
                d->grid[newY][newX].occupant = p->type;
                return 1;
            }
            isAttacked = 1;
        }
        else
        {
            isNPC = 1;
        }
    }

    int newHard;
    if (d->grid[newY][newX].type == rockC)
    {
        newHard = d->grid[newY][newX].hardness - 85;

        if (newHard <= 0)
        {
            d->grid[newY][newX].hardness = 0;
            d->grid[newY][newX].type = corridorC;
            // pos and both Maps change
            resetNTMap;
            resetTMap;
        }
        else
        {
            d->grid[newY][newX].hardness = newHard;
            // pos does not change but the TMap changes
            p->turn += (1000 / p->speed);
            p->hn = heap_insert(h, p);
            resetTMap;
            return 0;
        }
    }

    // making current cell unoccupied
    if (!isAttacked)
    {
        if (isNPC)
        {
            // NPC does not kill other NPC
            d->grid[y][x].occupant = d->grid[newY][newX].occupant;
            updateDragLoc(x, y, newX, newY, d);
        }
        else
        {
            d->grid[y][x].occupant = '*';
        }

        d->grid[newY][newX].occupant = p->type;
        p->pos = pos;
    }

    p->turn += (1000 / p->speed);
    p->hn = heap_insert(h, p);

    return 0;
}

/**
 * Unused
 */
void clean_occupants(std::vector<occupant_t *> &occupants)
{
    for (auto occupant : occupants)
    {
        delete occupant;
    }
    occupants.clear();
}

/*
Deprecated Method, an alternative method with UI is implemented in GameInterface.C
*/
// end game when only player remained (win) or when player got killed (lose)
void start_game(Dungeon *d)
{
    heap_t h;
    heap_init(&h, occupant_cmp, NULL);
    setOccupants(d, &h);

    // intially dungeon image
    printDungeon(d);
    usleep(3);
    occupant_t *p;

    while ((p = (occupant_t *)heap_remove_min(&h)))
    {
        // certain occupants killed and updated the occupant array as there heap node set to point null so they should allow to move or reinsert.
        if (p->hn)
        {
            if (p->type == PLAYER)
            {
                player_move(d, (player_t *)p, &h);
                printDungeon(d);
                usleep(250000);
                if (h.size == 1)
                {
                    printf("\n%s\n", "Player won the game!");
                    return;
                }
            }
            else
            {
                if (dragon_move(d, (dragon_t *)p, &h))
                {
                    heap_delete(&h);
                    printDungeon(d);
                    clean_occupants(d->occupants);
                    printf("\n%s\n", "Player lost the game!");
                    return;
                }
            }
        }
    }
}
