#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <float.h>
#include <math.h>
#include <getopt.h>
#include <string.h>
#include "DungeonGen.h"
#include "DungeonLogistic.h"
#include "FetchPath.h"
#include "Movement.h"
#include "GameInterface.h"
#include "Fog.h"
#include <vector>
#include "descriptions.h"
#include "util.h"
int SAVE = 0;
int LOAD = 0;

// default
int Dragon_Num = 25;
int Obj_Num = 25;

struct option commads[] = {
    {"save", no_argument, &SAVE, 1}, {"load", no_argument, &LOAD, 1}, {"nummon", required_argument, NULL, 'd'}, {0, 0, 0, 0}};

void processCommand(int argc, char *argv[])
{
    int opt;

    while ((opt = getopt_long(argc, argv, "", commads, NULL)) != -1)
    {
        switch (opt)
        {
        case 'd':
            // currently number of dragons and object are same
            Dragon_Num = atoi(optarg);
            Obj_Num = Dragon_Num;
            break;
        case 0: // for save and load
            break;
        case '?': // invalid option
            break;
        }
    }
}

int main(int argc, char *argv[])
{
    char *dungeon_file_path = NULL;
    srand(time(NULL));
    processCommand(argc, argv);

    Dungeon dungeon;

    getDirPath(&dungeon_file_path);

    if (LOAD)
    {
        load(&dungeon, dungeon_file_path);
    }
    else
    {
        // malloc only assigns the memory, whereas realloc will assign and initialise
        dungeon.rooms = (Room *)malloc(sizeof(Room) * MAX_ROOMS);
        if (dungeon.rooms == NULL)
        {
            printf("Memory allocation failed for dungeon rooms\n");
        }
        memset(dungeon.rooms, 0, sizeof(Room) * MAX_ROOMS);

        initializeDungeon(&dungeon);
        fitRooms(&dungeon);
        connect_rooms(&dungeon);
        fitMinStair(&dungeon);
    }

    if (SAVE)
    {
        save(&dungeon, dungeon_file_path);
    }

    parse_descriptions(&dungeon);
    // print_descriptions(&dungeon);
    findMaxObjAndDragon(&dungeon);
    
    if (Dragon_Num <= dungeon.maxDragon)
    {
        dungeon.DragonNum = Dragon_Num;
    }
    else
    {
        dungeon.DragonNum = dungeon.maxDragon;
    }
    if (Obj_Num <= dungeon.maxObj)
    {
        dungeon.ObjNum = Obj_Num;
    }
    else
    {
        dungeon.ObjNum = dungeon.maxObj;
    }
    
    // intialising the statically allocated 2d array
    memset(dungeon.visitedGrid, 0, sizeof(dungeon.visitedGrid));
    markVisited(&dungeon, dungeon.PlayerPos);

    // call UI method
    dungeon_UI(&dungeon);

    destroy_descriptions(&dungeon);
    free(dungeon.rooms);
    free(dungeon_file_path);

    return 0;
}
