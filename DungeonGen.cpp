#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <float.h>
#include <math.h>
#include "DungeonGen.h"
#include "Movement.h"
void initializeDungeon(Dungeon *dungeon)
{
    dungeon->roomCount = 0;
    dungeon->stairCount = 0;
    for (int y = 0; y < DUNGEON_H; y++)
    {
        for (int x = 0; x < DUNGEON_W; x++)
        {
            if (y == 0 || y == DUNGEON_H - 1 || x == 0 || x == DUNGEON_W - 1)
            {
                dungeon->grid[y][x].type = immutableRock;
                dungeon->grid[y][x].hardness = 255;
            }
            else
            {
                dungeon->grid[y][x].type = rockC;
                dungeon->grid[y][x].hardness = 1 + rand() % 255;
            }
            dungeon->grid[y][x].occupant = '*';
            dungeon->grid[y][x].obj = objtype_no_type;
        }
    }
}

/*
old method to print in terminal
*/
void printDungeon(Dungeon *dungeon)
{
    for (int y = 0; y < DUNGEON_H; y++)
    {
        for (int x = 0; x < DUNGEON_W; x++)
        {

            if (dungeon->grid[y][x].occupant != '*')
            {
                if (dungeon->grid[y][x].occupant == PLAYER)
                {
                    printf("@");
                }
                else
                {
                    printf("%x", dungeon->grid[y][x].occupant);
                }
            }
            else
            {
                switch (dungeon->grid[y][x].type)
                {
                case rockC:
                    printf(" ");
                    break;
                case immutableRock:
                    printf(" ");
                    break;
                case roomC:
                    printf(".");
                    break;
                case corridorC:
                    printf("#");
                    break;
                case upStairC:
                    printf("<");
                    break;
                case downStairC:
                    printf(">");
                    break;
                default:
                    // to track any cell missed to initialize
                    printf("?");
                }
            }
        }
        printf("\n");
    }
}

int roomPossible(Dungeon *dungeon, Room *room)
{
    if (room->srow < 1 || room->scol < 1 || room->srow + room->height >= DUNGEON_H - 1 || room->scol + room->width >= DUNGEON_W - 1)
    {
        return 0;
    }
    //+1 and -1 over dimensions ensure one cell space between rooms
    for (int y = room->srow - 1; y < room->srow + room->height + 1; y++)
    {
        for (int x = room->scol - 1; x < room->scol + room->width + 1; x++)
        {
            if (dungeon->grid[y][x].type == roomC)
            {
                return 0;
            }
        }
    }
    return 1;
}
void constructRoom(Dungeon *dungeon, Room *room)
{
    for (int y = room->srow; y < room->srow + room->height; y++)
    {
        for (int x = room->scol; x < room->scol + room->width; x++)
        {
            dungeon->grid[y][x].type = roomC;
            dungeon->grid[y][x].hardness = 0;
        }
    }
}

void fitRooms(Dungeon *dungeon)
{
    Room room;
    int counter = 0;
    while (dungeon->roomCount < MAX_ROOMS)
    {
        int addLen = rand() % ROOM_MIN_W;
        room.width = ROOM_MIN_W + addLen;
        room.height = ROOM_MIN_H + addLen;
        room.scol = 1 + rand() % (DUNGEON_W - room.width - 2);
        room.srow = 1 + rand() % (DUNGEON_H - room.height - 2);

        if (roomPossible(dungeon, &room))
        {
            constructRoom(dungeon, &room);
            dungeon->rooms[dungeon->roomCount++] = room;
        }
        if (dungeon->roomCount >= MIN_ROOMS && counter >= 10)
        {
            break;
        }
        counter++;
    }
}

Point findConnectingSideOfRoom(Room r, Point roomCenter, Point target)
{
    Point p;
    int dx = target.x - roomCenter.x;
    int dy = target.y - roomCenter.y;
    if (abs(dy) > abs(dx))
    {
        if (dy > 0)
        {
            p.y = r.srow + r.height;
        }
        else
        {
            p.y = r.srow - 1;
        }
        p.x = roomCenter.x;
    }
    else
    {
        if (dx > 0)
        {
            p.x = r.scol + r.width;
        }
        else
        {
            p.x = r.scol - 1;
        }
        p.y = roomCenter.y;
    }

    return p;
}
void carveCorridor(Dungeon *dungeon, int startRoom, int destRoom)
{
    Room start = dungeon->rooms[startRoom];
    Room end = dungeon->rooms[destRoom];

    Point startCenter = {start.scol + start.width / 2, start.srow + start.height / 2};
    Point endCenter = {end.scol + end.width / 2, end.srow + end.height / 2};

    Point sp = findConnectingSideOfRoom(start, startCenter, endCenter);

    int x, y;
    x = sp.x;
    y = sp.y;
    int tempX = x;
    int tempY = y;
    int stairDeterminer;
    while (!((x >= end.scol && x <= end.scol + end.width - 1) && (y >= end.srow && y <= end.srow + end.height - 1)))
    {
        if (dungeon->grid[y][x].type == rockC)
        {
            dungeon->grid[y][x].type = corridorC;

            if (dungeon->grid[y - 1][x].type != roomC && dungeon->grid[y + 1][x].type != roomC && dungeon->grid[y][x - 1].type != roomC && dungeon->grid[y][x + 1].type != roomC)
            {
                stairDeterminer = rand() % 100;
                if (stairDeterminer == 82 && dungeon->stairCount < 5)
                {
                    dungeon->grid[y][x].type = upStairC;
                    dungeon->stairCount++;
                }
                else if (stairDeterminer == 21 && dungeon->stairCount < 5)
                {
                    dungeon->grid[y][x].type = downStairC;
                    dungeon->stairCount++;
                }
            }
            dungeon->grid[y][x].hardness = 0;
        }

        if (x < endCenter.x && x < DUNGEON_W - 2)
        {
            tempX = x + 1;
        }
        else if (x > endCenter.x && x > 2)
        {
            tempX = x - 1;
        }

        if (y < endCenter.y && y < DUNGEON_H - 2)
        {
            tempY = y + 1;
        }
        else if (y > endCenter.y && y > 2)
        {
            tempY = y - 1;
        }

        if (x != endCenter.x && y != endCenter.y)
        {
            if (dungeon->grid[y][tempX].hardness <= dungeon->grid[tempY][x].hardness)
            {
                x = tempX;
            }
            else
            {
                y = tempY;
            }
        }
        else
        {
            x = tempX;
            y = tempY;
        }
    }
}

void connect_rooms(Dungeon *dungeon)
{
    double x1;
    double y1;
    double x2;
    double y2;
    double dist;
    int connected[dungeon->roomCount];
    for (int i = 0; i < dungeon->roomCount; i++)
    {
        connected[i] = 0;
    }
    connected[0] = 1;

    for (int i = 1; i < dungeon->roomCount; i++)
    {

        int closest = -1;
        double closest_dist = DBL_MAX;

        for (int j = 0; j < dungeon->roomCount; j++)
        {
            if (!connected[j])
            {
                continue;
            }
            x1 = dungeon->rooms[i].scol + dungeon->rooms[i].width / 2;
            y1 = dungeon->rooms[i].srow + dungeon->rooms[i].height / 2;

            x2 = dungeon->rooms[j].scol + dungeon->rooms[j].width / 2;
            y2 = dungeon->rooms[j].srow + dungeon->rooms[j].height / 2;

            dist = sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2));

            if (dist < closest_dist)
            {
                closest_dist = dist;
                closest = j;
            }
        }
        if (closest != -1)
        {
            carveCorridor(dungeon, i, closest);
            connected[i] = 1;
        }
    }
}

void fitMinStair(Dungeon *dungeon)
{

    int totalrooms = dungeon->roomCount;

    int rNo = rand() % totalrooms;
    int r1col = dungeon->rooms[rNo].scol;
    int r1row = dungeon->rooms[rNo].srow;
    rNo = rand() % (totalrooms - 1);
    int r2col = dungeon->rooms[rNo].scol;
    int r2row = dungeon->rooms[rNo].srow;

    // rooms minW=4, minH=3

    dungeon->grid[r1row + 1][r1col + 1].type = upStairC;
    dungeon->grid[r1row + 1][r1col + 1].hardness = 0;

    dungeon->grid[r2row + 1][r2col + 2].type = downStairC;
    dungeon->grid[r2row + 1][r2col + 2].hardness = 0;

    dungeon->stairCount += 2;

    // fit player at default place
    dungeon->grid[dungeon->rooms[0].srow][dungeon->rooms[0].scol].occupant = PLAYER;
    dungeon->PlayerPos.x = dungeon->rooms[0].scol;
    dungeon->PlayerPos.y = dungeon->rooms[0].srow;
}
