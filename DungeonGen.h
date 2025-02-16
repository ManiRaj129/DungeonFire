#ifndef DUNGEONGEN_H
#define DUNGEONGEN_H

#define DUNGEON_W 80
#define DUNGEON_H 21
#define MIN_ROOMS 6
#define MAX_ROOMS 8
#define ROOM_MIN_W 4
#define ROOM_MIN_H 3

typedef struct
{
    int x, y;
} Point;

typedef enum
{
    roomC,
    corridorC,
    upStairC,
    downStairC,
    rockC,
    player
} CType;

typedef struct
{
    CType type;
    int hardness;
} Cell;

typedef struct
{
    int srow;
    int scol;
    int width;
    int height;
} Room;

typedef struct
{
    Cell grid[DUNGEON_H][DUNGEON_W];
    
    Room *rooms;

    int roomCount;
    int stairCount;
} Dungeon;

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