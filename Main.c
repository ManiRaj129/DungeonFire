#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <float.h>
#include <math.h>
#include <getopt.h>
#include <string.h>
#include "DungeonGen.h"
#include "DungeonLogistic.h"

int SAVE = 0;
int LOAD = 0;

struct option commads[] = {
    {"save", no_argument, &SAVE, 1}, {"load", no_argument, &LOAD, 1}, {0, 0, 0, 0}};

void processCommand(int argc, char *argv[])
{
    int opt;

    while ((opt = getopt_long(argc, argv, "", commads, NULL)) != -1)
    {

        switch (opt)
        {
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
        dungeon.rooms = malloc(sizeof(Room) * MAX_ROOMS);
        if (dungeon.rooms == NULL)
        {
            printf("Memory allocation failed for dungeon rooms\n");
        }
        memset(dungeon.rooms,0, sizeof(Room) * MAX_ROOMS);
        initializeDungeon(&dungeon);
        fitRooms(&dungeon);
        connect_rooms(&dungeon);
        fitMinStair(&dungeon);
    }
    
    printDungeon(&dungeon);

    if (SAVE)
    {
        save(&dungeon, dungeon_file_path);
    }

    free(dungeon.rooms);
    free(dungeon_file_path);

    return 0;
}