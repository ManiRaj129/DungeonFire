#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <time.h>
#include <float.h>
#include <string.h>
#include <math.h>
#include <endian.h>
#include "DungeonGen.h"
#include "Movement.h"
typedef struct
{
    uint8_t x, y;
} stair;

void getDirPath(char **dungeon_file_path)
{
    char *home;
    int dungeon_file_path_length;

    home = getenv("HOME");
    if (!home) // Check if getenv failed
    {
        printf("Failed to get HOME environment variable\n");
    }
    dungeon_file_path_length = strlen(home) + strlen("/.rlg327/09.rlg327") + 1; //+1 for null byte

    *dungeon_file_path = (char *)malloc(dungeon_file_path_length * sizeof(char));
    if (*dungeon_file_path == NULL)
    {
        printf("memory allocation failed for dungeon_file_path");
    }

    strcpy(*dungeon_file_path, home);
    strcat(*dungeon_file_path, "/.rlg327/09.rlg327");
    (*dungeon_file_path)[dungeon_file_path_length - 1] = '\0';
}

void processDungeon(Dungeon *dungeon, FILE *f, int isLoad)
{
    uint8_t val;

    for (int i = 0; i < DUNGEON_H; i++)
    {
        for (int j = 0; j < DUNGEON_W; j++)
        {
            if (isLoad)
            {
                if (!fread(&val, sizeof(uint8_t), 1, f))
                {
                    printf("%s\n", "hardess val load failed");
                }

                dungeon->grid[i][j].hardness = val;
                dungeon->grid[i][j].occupant = '*';
                if (val == 0)
                {
                    dungeon->grid[i][j].type = corridorC; // will assign appropriate openspace later
                }
                else
                {
                    if (val == 255)
                    {
                        dungeon->grid[i][j].type = immutableRock;
                    }
                    else
                    {
                        dungeon->grid[i][j].type = rockC;
                    }
                }
            }
            else
            {
                // save
                val = (uint8_t)dungeon->grid[i][j].hardness;
                fwrite(&val, sizeof(val), 1, f);
            }
        }
    }
}

void placeRoom(Dungeon *dungeon, Room room)
{

    for (int y = room.srow; y < room.srow + room.height; y++)
    {
        for (int x = room.scol; x < room.scol + room.width; x++)
        {
            dungeon->grid[y][x].type = roomC;
        }
    }
}

void processRooms(Dungeon *dungeon, FILE *f, int isLoad)
{
    uint8_t x;
    uint8_t y;
    uint8_t width;
    uint8_t height;

    // not filling th rooms array in dugeon
    for (int i = 0; i < dungeon->roomCount; i++)
    {
        if (isLoad)
        {
            if (!fread(&x, sizeof(x), 1, f))
            {
                printf("%s\n", "roomX load failed");
            }
            dungeon->rooms[i].scol = x;

            if (!fread(&y, sizeof(y), 1, f))
            {
                printf("%s\n", "roomY load failed");
            }
            dungeon->rooms[i].srow = y;

            if (!fread(&width, sizeof(width), 1, f))
            {
                printf("%s\n", "roomWidth load failed");
            }
            dungeon->rooms[i].width = width;

            if (!fread(&height, sizeof(height), 1, f))
            {
                printf("%s\n", "roomHeight load failed");
            }
            dungeon->rooms[i].height = height;

            placeRoom(dungeon, dungeon->rooms[i]);
        }
        else
        {
            x = (uint8_t)dungeon->rooms[i].scol;
            fwrite(&x, sizeof(x), 1, f);
            y = (uint8_t)dungeon->rooms[i].srow;
            fwrite(&y, sizeof(y), 1, f);
            width = (uint8_t)dungeon->rooms[i].width;
            fwrite(&width, sizeof(width), 1, f);
            height = (uint8_t)dungeon->rooms[i].height;
            fwrite(&height, sizeof(height), 1, f);
        }
    }
}

void placeStairs(Dungeon *dungeon, uint16_t stairCount, int isUpsatirs, FILE *f)
{
    // col, row
    uint8_t x, y;
    for (int i = 0; i < stairCount; i++)
    {
        if (!fread(&x, sizeof(x), 1, f))
        {
            printf("%s\n", "satirX load failed");
        }
        if (!fread(&y, sizeof(y), 1, f))
        {
            printf("%s\n", "satairY load failed");
        }
        if (isUpsatirs)
        {
            dungeon->grid[y][x].type = upStairC;
        }
        else
        {
            dungeon->grid[y][x].type = downStairC;
        }
    }
}

void load(Dungeon *dungeon, const char *dungeon_file_path)
{
    FILE *f;
    f = fopen(dungeon_file_path, "rb");
    if (f == NULL)
    {
        printf("failed to open file at %s\n", dungeon_file_path);
    }

    char marker[13];
    // essentially the address to the first char
    if (!fread(marker, sizeof(char), 12, f))
    {
        printf("%s\n", "marker load failed");
    }

    uint32_t version = 0;
    if (!fread(&version, sizeof(uint32_t), 1, f))
    {
        printf("%s\n", "version load failed");
    }
    version = be32toh(version);

    uint32_t fileSize = 0;

    if (!fread(&fileSize, sizeof(uint32_t), 1, f))
    {
        printf("%s\n", "fileSize load failed");
    }
    fileSize = be32toh(fileSize);

    uint8_t player_X = 0;
    if (!fread(&player_X, sizeof(uint8_t), 1, f))
    {
        printf("%s\n", "player_X load failed");
    }
    uint8_t player_Y = 0;
    if (!fread(&player_Y, sizeof(uint8_t), 1, f))
    {
        printf("%s\n", "player_Y load failed");
    }

    // placed player on grid after assigning rooms

    // fetch the hardness of cells
    processDungeon(dungeon, f, 1);

    uint16_t roomCount = 0;
    if (!fread(&roomCount, sizeof(uint16_t), 1, f))
    {
        printf("%s\n", "roomCount load failed");
    }
    roomCount = be16toh(roomCount);
    dungeon->roomCount = roomCount;

    dungeon->rooms = (Room *)malloc(sizeof(Room) * roomCount);
    if (dungeon->rooms == NULL)
    {
        printf("%s", "room allocation with malloc failed");
    }
    memset(dungeon->rooms, 0, sizeof(Room) * roomCount);

    // fetch rooms and the third argument specifies the load operation
    processRooms(dungeon, f, 1);
    dungeon->grid[player_Y][player_X].occupant = PLAYER;
    dungeon->PlayerPos.x = player_X;
    dungeon->PlayerPos.y = player_Y;

    // fetch upward stairs count
    uint16_t upStairCount = 0;
    if (!fread(&upStairCount, sizeof(uint16_t), 1, f))
    {
        printf("%s\n", "upStairCount load failed");
    }
    upStairCount = be16toh(upStairCount);

    // place all the upstairs
    placeStairs(dungeon, upStairCount, 1, f);

    // fetch down stairs count
    uint16_t downStairCount = 0;
    if (!fread(&downStairCount, sizeof(uint16_t), 1, f))
    {
        printf("%s\n", "downStairCount load failed");
    }
    downStairCount = be16toh(downStairCount);

    // place all the downstairs
    placeStairs(dungeon, downStairCount, 0, f);

    dungeon->stairCount = upStairCount + downStairCount;

    fclose(f);
}
void findStairs(Dungeon *dungeon, uint16_t *upStairCount, uint16_t *downStairCount, stair upStairs[], stair downStairs[])
{
    for (int y = 1; y < DUNGEON_H - 1; y++)
    {
        for (int x = 1; x < DUNGEON_W - 1; x++)
        {
            if (dungeon->grid[y][x].type == upStairC)
            {
                upStairs[*upStairCount].x = (uint8_t)x;
                upStairs[*upStairCount].y = (uint8_t)y;
                (*upStairCount)++;
            }
            else if (dungeon->grid[y][x].type == downStairC)
            {
                downStairs[*downStairCount].x = (uint8_t)x;
                downStairs[*downStairCount].y = (uint8_t)y;
                (*downStairCount)++;
            }
        }
    }
}
void saveStairs(stair stairs[], int stairCount, FILE *f)
{
    uint8_t x, y;
    for (int i = 0; i < stairCount; i++)
    {
        x = stairs[i].x;
        fwrite(&x, sizeof(x), 1, f);
        y = stairs[i].y;
        fwrite(&y, sizeof(y), 1, f);
    }
}
void save(Dungeon *dungeon, const char *dungeon_file_path)
{
    FILE *f;
    f = fopen(dungeon_file_path, "wb");
    if (f == NULL)
    {
        printf("failed to open file at %s\n", dungeon_file_path);
    }

    const char *marker = "RLG327-S2025";
    fwrite(marker, sizeof(char), 12, f);

    uint32_t version = 0;
    version = htobe32(version);
    fwrite(&version, sizeof(version), 1, f);

    // 1704+ room bytes+ upstair count byte + down stair count byte+ total stair info bytes
    uint32_t fileSize = 1708 + (dungeon->roomCount * 4) + (dungeon->stairCount * 2);
    fileSize = htobe32(fileSize);
    fwrite(&fileSize, sizeof(fileSize), 1, f);

    // process the entire dungeon to fetch up and down stair info
    uint16_t upStairCount = 0;
    uint16_t downStairCount = 0;
    stair upstairs[dungeon->stairCount];
    stair downstairs[dungeon->stairCount];
    findStairs(dungeon, &upStairCount, &downStairCount, upstairs, downstairs);

    uint8_t player_X = dungeon->PlayerPos.x;
    uint8_t player_Y = dungeon->PlayerPos.y;
    fwrite(&player_X, sizeof(player_X), 1, f);
    fwrite(&player_Y, sizeof(player_Y), 1, f);

    // fetch the hardness of cells
    processDungeon(dungeon, f, 0);

    uint16_t roomCount = (uint16_t)dungeon->roomCount;
    roomCount = htobe16(roomCount);
    fwrite(&roomCount, sizeof(roomCount), 1, f);

    // fetch rooms and the third argument specifies the save operation
    processRooms(dungeon, f, 0);

    // up stairs
    int upSC = upStairCount;
    upStairCount = htobe16(upStairCount);
    fwrite(&upStairCount, sizeof(upStairCount), 1, f);

    saveStairs(upstairs, upSC, f);

    // down stairs
    int downSC = downStairCount;
    downStairCount = htobe16(downStairCount);
    fwrite(&downStairCount, sizeof(downStairCount), 1, f);

    saveStairs(downstairs, downSC, f);

    fclose(f);
}
