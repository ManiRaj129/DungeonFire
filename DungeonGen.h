#ifndef DUNGEONGEN_H
#define DUNGEONGEN_H

#define DUNGEON_W 80
#define DUNGEON_H 21
#define MIN_ROOMS 6
#define MAX_ROOMS 8
#define ROOM_MIN_W 4
#define ROOM_MIN_H 3
#define MONSTER_DESC_FILE "monster_desc.txt"
#define OBJECT_DESC_FILE "object_desc.txt"
#define SAVE_DIR ".rlg327"

#include <vector>
#include "descriptions.h"
#include "objectTypes.h"
#include "object.h"


typedef struct
{
    int x, y;
} Point;

typedef enum
{
    roomC = 1,
    corridorC,
    upStairC,
    downStairC,
    rockC,
    immutableRock
} CType;

typedef struct
{
    CType type;
    int hardness;
    // '*' in abscence of occupant.
    char occupant;
    object_type_t obj;
} Cell;

typedef struct
{
    int srow;
    int scol;
    int width;
    int height;
} Room;

// forward declaration to prevent circular dependency  only works when only pointers of the declared component used
class occupant_t;

class Dungeon
{
public:
    int visitedGrid[DUNGEON_H][DUNGEON_W];
    Cell grid[DUNGEON_H][DUNGEON_W];
    Room *rooms;
    /*occupant_t *occupants;  is not suitable for polymorphism
     * where we can use: occupant_t **occupants;
     * */
    std::vector<occupant_t *> occupants;
    Point PlayerPos;
    int roomCount;
    int stairCount;
    int DragonNum;
    int ObjNum;
    int maxDragon;
    int maxObj;

    std::vector<object> objects;
    std::vector<monster_description> monster_descriptions;
    std::vector<object_description> object_descriptions;
};

void initializeDungeon(Dungeon *dungeon);
void printDungeon(Dungeon *dungeon);
int roomPossible(Dungeon *dungeon, Room *room);
void constructRoom(Dungeon *dungeon, Room *room);
void fitRooms(Dungeon *dungeon);
Point findConnectingSideOfRoom(Room r, Point roomCenter, Point target);
void carveCorridor(Dungeon *dungeon, int startRoom, int destRoom);
void connect_rooms(Dungeon *dungeon);
void fitMinStair(Dungeon *dungeon);

#endif
