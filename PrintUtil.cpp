#include "PrintUtil.h"
#include <ncurses.h>
#include "Movement.h"
#include <stdlib.h>
#include "objectTypes.h"
#include "descriptions.h"


int getDragonColor(Dungeon *d, char type)
{

    for (int i = 0; i < (int)d->monster_descriptions.size(); i++)
    {
        if (type == d->monster_descriptions[i].get_symbol())
        {
            return d->monster_descriptions[i].get_color()[0];
        }
    }

    return 0;
}

int getObjectColor(Dungeon *d, object_type_t type)
{

    for (int i = 0; i < (int)d->objects.size(); i++)
    {
        if (type == d->objects[i].type)
        {
            return d->objects[i].color;
        }
    }

    return 0;
}

/**
 * prints full dungeon without fog on Ncurses Screen
 */
void printDungeonNC(Dungeon *dungeon)
{
    int color;
    for (int y = 0; y < DUNGEON_H; y++)
    {
        // dungeon starts from row 1

        for (int x = 0; x < DUNGEON_W; x++)
        {
            move(y + 1, x);
            if (dungeon->grid[y][x].occupant != '*')
            {
                if (dungeon->grid[y][x].occupant == PLAYER)
                {
                    printw("@");
                }
                else
                {
                    color = getDragonColor(dungeon, dungeon->grid[y][x].occupant);
                    attron(COLOR_PAIR(color));

                    printw("%c", dungeon->grid[y][x].occupant);
                    attroff(COLOR_PAIR(color));
                }
            }
            else if (dungeon->grid[y][x].obj != objtype_no_type)
            {

                color = getObjectColor(dungeon, dungeon->grid[y][x].obj);
                attron(COLOR_PAIR(color));

                printw("%c", object_symbol[dungeon->grid[y][x].obj]);
                attroff(COLOR_PAIR(color));
            }
            else
            {
                switch (dungeon->grid[y][x].type)
                {
                case rockC:
                    printw(" ");
                    break;
                case immutableRock:
                    printw(" ");
                    break;
                case roomC:
                    printw(".");
                    break;
                case corridorC:
                    printw("#");
                    break;
                case upStairC:
                    printw("<");
                    break;
                case downStairC:
                    printw(">");
                    break;
                default:
                    printw("?");
                }
            }
        }
    }
    refresh();
}

void printVisible(Dungeon *dungeon)
{
    int color;

    for (int y = 0; y < DUNGEON_H; y++)
    {

        for (int x = 0; x < DUNGEON_W; x++)
        {
            move(y + 1, x);
            // defalut cell value is false
            if (!dungeon->visitedGrid[y][x])
            {
                printw(" ");
            }
            else
            {
                if (dungeon->grid[y][x].occupant != '*')
                {
                    if (dungeon->grid[y][x].occupant == PLAYER)
                    {
                        printw("@");
                    }
                    else if (isVisible(x, y, dungeon->PlayerPos.x, dungeon->PlayerPos.y))
                    {
                        color = getDragonColor(dungeon, dungeon->grid[y][x].occupant);
                        attron(COLOR_PAIR(color));

                        printw("%c", dungeon->grid[y][x].occupant);
                        attroff(COLOR_PAIR(color));
                    }
                    else
                    {
                        printw(" ");
                    }
                }
                else if (dungeon->grid[y][x].obj != objtype_no_type)
                {

                    color = getObjectColor(dungeon, dungeon->grid[y][x].obj);
                    attron(COLOR_PAIR(color));

                    printw("%c", object_symbol[dungeon->grid[y][x].obj]);
                    attroff(COLOR_PAIR(color));
                }
                else
                {
                    switch (dungeon->grid[y][x].type)
                    {
                    case rockC:
                        printw(" ");
                        break;
                    case immutableRock:
                        printw(" ");
                        break;
                    case roomC:
                        printw(".");
                        break;
                    case corridorC:
                        printw("#");
                        break;
                    case upStairC:
                        printw("<");
                        break;
                    case downStairC:
                        printw(">");
                        break;
                    default:
                        printw("?");
                    }
                }
            }
        }
    }

    refresh();
}