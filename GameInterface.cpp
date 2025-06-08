
#include <ncurses.h>
#include "Movement.h"
#include "heap.h"
#include "GameInterface.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include "Fog.h"
#include "Teleport.h"
#include "PrintUtil.h"
#include "util.h"
#include "prompt.h"
#include "targetMode.h"

void reset_dungeon(Dungeon *dungeon, heap_t *h)
{
    heap_delete(h);
    int dragonNum = dungeon->DragonNum;

    // initializeDungeon(dungeon);
    fitRooms(dungeon);
    connect_rooms(dungeon);
    // places minimum stairs and also player at default position in dungeon
    fitMinStair(dungeon);

    // updating the player visited terrain
    memset(dungeon->visitedGrid, 0, sizeof(dungeon->visitedGrid));
    markVisited(dungeon, dungeon->PlayerPos);

    dungeon->DragonNum = dragonNum;

    resetDragonAndObjCount(dungeon);
    setOccupants(dungeon, h);
    set_objects(dungeon);
}

/*
  At most 21 mosters are displayed at a time
*/
void display_list(Dungeon *d, int sindex)
{

    int i = sindex;
    // directions with respect to player
    int verticalD = 0;
    int horizontalD = 0;
    char direction[30];
    direction[0] = '\0';
    int j = 0;
    // occupants count dragonNum + 1 player where i is for occupants
    while (j < 21 && i <= d->DragonNum)
    {
        move(j + 1, 0);
        clrtoeol();

        if (d->occupants[i]->type != '@')
        {
            // if true it is still alive
            if (d->grid[d->occupants[i]->pos.y][d->occupants[i]->pos.x].occupant == d->occupants[i]->type)
            { // vertical
                verticalD = d->PlayerPos.y - d->occupants[i]->pos.y;
                horizontalD = d->PlayerPos.x - d->occupants[i]->pos.x;

                // dragon character
                sprintf(direction + strlen(direction), "%x,", d->occupants[i]->type);

                // when equal to zero no need to specify
                if (verticalD > 0)
                {
                    // north
                    sprintf(direction + strlen(direction), " %d north", verticalD);
                }
                else if (verticalD < 0)
                {
                    // south
                    sprintf(direction + strlen(direction), " %d south", abs(verticalD));
                }

                if (horizontalD > 0)
                {
                    if (strlen(direction) > 2)
                    {
                        sprintf(direction + strlen(direction), " and");
                    }
                    // west
                    sprintf(direction + strlen(direction), " %d west", horizontalD);
                }
                else if (horizontalD < 0)
                {
                    if (strlen(direction) > 2)
                    {
                        sprintf(direction + strlen(direction), " and");
                    }
                    // east
                    sprintf(direction + strlen(direction), " %d east", abs(horizontalD));
                }

                printw("%s", direction);
                direction[0] = '\0';
                verticalD = 0;
                horizontalD = 0;
                j++;
            }
        }
        i++;
    }
    refresh();
}

void display_monsters(Dungeon *d)
{
    // clear the screen
    clear();
    refresh();
    move(0, 0);
    printw("%s", "List of alive dragons and their position with respect to your location (player)");
    int start_index = 0;
    display_list(d, start_index);

    int command;
    command = getch();

    // 27: ASCII for ESC
    while (command != 27)
    {
        move(0, 0);
        clrtoeol();
        printw("%s", "List of alive dragons and their position with respect to your location (player)");

        // down and up arrows to scroll
        if (command == KEY_UP)
        {

            if (start_index > 0)
            {
                start_index--;
                display_list(d, start_index);
            }
            else
            {
                move(0, 0);
                clrtoeol();
                printw("%s", "Top reached");
                refresh();
            }
        }
        else if (command == KEY_DOWN)
        {

            if (start_index + 21 < d->DragonNum)
            {
                start_index++;
                display_list(d, start_index);
            }
            else
            {
                move(0, 0);
                clrtoeol();
                printw("%s", "Bottom reached");
                refresh();
            }
        }

        command = getch();
    }
}

// new player movemnet which moves by user input
/*

  Note: player gets doubles when used stair commands
  returns -1 if quitted, 0 if moved or killed normal dragon, and 1 if killed (Boss)
 */
int move_player(Dungeon *d, player_t *p, heap_t *h)
{

    // check for stairs and let the user know they are on staircase
    if (d->grid[d->PlayerPos.y][d->PlayerPos.x].type == downStairC)
    {
        move(0, 0);
        clrtoeol();
        printw("%s", "You are currently on Down stair case. Make any possible Move you wish!");
        refresh();
    }
    else if (d->grid[d->PlayerPos.y][d->PlayerPos.x].type == upStairC)
    {
        move(0, 0);
        clrtoeol();
        printw("%s", "You are currently on Up stair case. Make any possible Move you wish!");
        refresh();
    }
    else
    {
        move(0, 0);
        clrtoeol();
        printw("%s", "Make any possible Move you wish!");
        refresh();
    }

    int command = getch();

    int isNotMoved = 1;

    // default
    int newX = 0;
    int newY = 0;
    // adjust later based on input
    int x_move = 0;
    int y_move = 0;

    int isChecked = 0;

    while (isNotMoved)
    {
        move(0, 0);
        clrtoeol();
        printw("%s", "Make any possible Move you wish!");
        refresh();

        if (command == 'Q')
        {
            return -1;
        }

        if (command == 'm')
        {
            display_monsters(d);

            clear();
            printVisible(d);
            move(0, 0);
            clrtoeol();
            printw("%s", "Make any possible Move you wish!");
            refresh();

            // after exiting from display
            command = getch();
        }
        else if (command == 'i')
        {
            carryList(p->carry);

            command = getch();
            while (command != 27)
            {
                move(0, 0);
                clrtoeol();
                printw("%s", "Enter escape to get back to the game");
                refresh();
                command = getch();
            }

            clear();
            printVisible(d);
            move(0, 0);
            clrtoeol();
            printw("%s", "Make any possible Move you wish!");
            refresh();

            // after exiting from display
            command = getch();
        }
        else if (command == 'e')
        {
            equipList(p->equipped);

            command = getch();
            while (command != 27)
            {
                move(0, 0);
                clrtoeol();
                printw("%s", "Enter escape to get back to the game");
                refresh();
                command = getch();
            }

            clear();
            printVisible(d);
            move(0, 0);
            clrtoeol();
            printw("%s", "Make any possible Move you wish!");
            refresh();

            // after exiting from display
            command = getch();
        }
        else if (command == 'w')
        {
            if ((int)p->equipped.size() < MaxEquip)
            {
                int selection = promptCarry(p->carry);
                if (selection != -1)
                {
                    wear(p, selection);
                }

                clear();
                printVisible(d);
                move(0, 0);
                clrtoeol();
                printw("%s", "Make any possible Move you wish!");
                refresh();
            }
            else
            {
                clear();
                printVisible(d);
                move(0, 0);
                clrtoeol();
                printw("%s", "Already equipped max objects(12),either continue game or take off any to equip new.");
                refresh();
            }
            // after exiting from display
            command = getch();
        }
        else if (command == 't')
        {
            if ((int)p->carry.size() < MaxCarry)
            {
                int selection = promptEquip(p->equipped);
                if (selection != -1)
                {
                    p->carry.push_back(p->equipped[selection]);
                    p->equipped.erase(p->equipped.begin() + selection);
                }

                clear();
                printVisible(d);
                move(0, 0);
                clrtoeol();
                printw("%s", "Make any possible Move you wish!");
                refresh();
            }
            else
            {
                clear();
                printVisible(d);
                move(0, 0);
                clrtoeol();
                printw("%s", "Carry is FUll (contains 10), either continue game or drop any object to take off.");
                refresh();
            }
            // after exiting from display
            command = getch();
        }
        else if (command == 'd')
        {
            int selection = promptCarry(p->carry);
            if (selection != -1)
            {
                d->objects.push_back(p->carry[selection]);
                // mark on cell
                d->grid[d->PlayerPos.y][d->PlayerPos.x].obj = p->carry[selection].type;
                p->carry.erase(p->carry.begin() + selection);
            }
            clear();
            printVisible(d);
            move(0, 0);
            clrtoeol();
            printw("%s", "dropped, Now make move! ");
            refresh();
            command = getch();
        }
        else if (command == 'x')
        {
            int selection = promptCarry(p->carry);
            if (selection != -1)
            { // permanently deleted
                abandonObj(d, p->carry, selection);
            }
            clear();
            printVisible(d);
            move(0, 0);
            clrtoeol();
            printw("%s", "Removed permanently, Now make move! ");
            refresh();
            command = getch();
        }
        else if (command == 'I')
        {
            int selection = promptCarry(p->carry);
            if (selection != -1)
            {
                // display item desc
                clear();
                move(0, 0);
                printw("%s", p->carry[selection].name.c_str());
                move(1, 0);
                printw("%s", p->carry[selection].desc.c_str());
                refresh();

                command = getch();
                while (command != 27)
                {
                    move(0, 0);
                    clrtoeol();
                    printw("%s", "Enter escape to get back to the game");
                    refresh();
                    command = getch();
                }
            }

            clear();
            printVisible(d);
            move(0, 0);
            clrtoeol();
            printw("%s", "make move");
            refresh();
            command = getch();
        }
        else if (command == 'L')
        {
            clear();
            // targeting mode
            targetAt(d);

            clear();
            printVisible(d);
            move(0, 0);
            clrtoeol();
            printw("%s", "make move");
            refresh();
            command = getch();
        }
        else if (command == 'f')
        {
            // full dungeon withoug fog
            printDungeonNC(d);

            // to make move after displaying without fog
            command = getch();
        }
        else if (command == 'g')
        {
            isChecked = 1;
            isNotMoved = 0;
            // get possible newX and newY that are in bound
            Point toCell = teleportTo(d);
            newX = toCell.x;
            newY = toCell.y;
        }
        // stairs
        else if (command == '>')
        {
            if (d->grid[d->PlayerPos.y][d->PlayerPos.x].type == upStairC)
            {
                move(0, 0);
                clrtoeol();
                printw("%s", "Wrong stair command, you are on down stair, try again!");
                refresh();

                command = getch();
            }
            else
            {
                reset_dungeon(d, h);
                // reallocated dungeon means made a move no need for any further adjustments

                return 1;
            }
        }
        else if (command == '<')
        {
            if (d->grid[d->PlayerPos.y][d->PlayerPos.x].type == downStairC)
            {
                move(0, 0);
                clrtoeol();
                printw("%s", "Wrong stair command, you are on Up stair, try again!");
                refresh();

                command = getch();
            }
            else
            {
                reset_dungeon(d, h);
                // reallocated dungeon means made a move no need for any further adjustments
                return 1;
            }
        }
        // Move up-left
        else if (command == '7' || command == 'y')
        {
            y_move -= 1;
            x_move -= 1;

            // assign false :0
            isNotMoved = 0;
        }
        // Move up
        else if (command == '8' || command == 'k')
        {
            y_move -= 1;
            // x does not change

            isNotMoved = 0;
        }
        // Move up right
        else if (command == '9' || command == 'u')
        {
            y_move -= 1;
            x_move += 1;
            isNotMoved = 0;
        }
        // Move right
        else if (command == '6' || command == 'l')
        {
            // y does not change as same row
            x_move += 1;
            isNotMoved = 0;
        }
        // Move down right
        else if (command == '3' || command == 'n')
        {
            y_move += 1;
            x_move += 1;
            isNotMoved = 0;
        }
        // Move down
        else if (command == '2' || command == 'j')
        {
            y_move += 1;
            // x does not change
            isNotMoved = 0;
        }
        // Move down left
        else if (command == '1' || command == 'b')
        {
            y_move += 1;
            x_move -= 1;
            isNotMoved = 0;
        }
        // Move left
        else if (command == '4' || command == 'h')
        {
            // y does not change
            x_move -= 1;
            isNotMoved = 0;
        }
        // rest
        else if (command == ' ' || command == '5' || command == '.')
        {
            // both x and y does not change
            isNotMoved = 0;
            isChecked = 1;
            newX = d->PlayerPos.x;
            newY = d->PlayerPos.y;
        }
        // invalid
        else
        {
            move(0, 0);
            clrtoeol();
            printw("%s", "invalid command, try again!");
            move(d->PlayerPos.y, d->PlayerPos.x);
            refresh();

            command = getch();
        }

        // when moved
        if (!isNotMoved && !isChecked)
        {
            newX = d->PlayerPos.x + x_move;
            newY = d->PlayerPos.y + y_move;

            if (!isInBound(newX, newY))
            {
                isNotMoved = 1;
                move(0, 0);
                clrtoeol();
                printw("%s", "invalid move, it moves out of bounds, try again!");
                refresh();
                command = getch();

                // default
                x_move = 0;
                y_move = 0;
            }

            // check for wall (normal rock)  the above condition includes the immutable rock and flags
            if (d->grid[newY][newX].type == rockC)
            {
                isNotMoved = 1;
                move(0, 0);
                clrtoeol();
                printw("%s", "invalid move, it's a wall, try again!");
                refresh();
                command = getch();

                x_move = 0;
                y_move = 0;
            }
        }
    }

    int isBossKilled = 0;
    int isAttacked = 0;
    // make move
    Point posNew = {newX, newY};

    // if there is an object
    if (d->grid[newY][newX].obj != objtype_no_type)
    {
        if (p->carry.size() < MaxCarry)
        {
            pickUp(d, p->carry, d->objects, posNew);
        }
    }

    // when there is any dragon
    if (d->grid[newY][newX].occupant != '*')
    {
        isAttacked = 1;
        isBossKilled = killOccupant(posNew, d, h);
        move(0, 0);
        clrtoeol();

        if (isBossKilled == 1)
        {
            printw("%s", "You killed Boss Dragon!");
            refresh();
            return 1;
        }
    }

    // vacate current cell; where non boss killed or normal move
    if (!isAttacked || (isAttacked && isBossKilled == 0))
    {
        d->grid[d->PlayerPos.y][d->PlayerPos.x].occupant = '*';
        d->grid[newY][newX].occupant = '@';
        d->PlayerPos = posNew;
        p->pos = posNew;
        markVisited(d, posNew);
    }

    p->turn += (1000 / p->speed);
    p->hn = heap_insert(h, p);

    return 0;
}

void dungeon_UI(Dungeon *d)
{
    heap_t h;

    setOccupants(d, &h);
    set_objects(d);

    initscr();
    cbreak();
    keypad(stdscr, TRUE);
    noecho();
    curs_set(0);
    start_color();
    init_pair(COLOR_RED, COLOR_RED, COLOR_BLACK);
    init_pair(COLOR_GREEN, COLOR_GREEN, COLOR_BLACK);
    init_pair(COLOR_YELLOW, COLOR_YELLOW, COLOR_BLACK);
    init_pair(COLOR_BLUE, COLOR_BLUE, COLOR_BLACK);
    init_pair(COLOR_MAGENTA, COLOR_MAGENTA, COLOR_BLACK);
    init_pair(COLOR_CYAN, COLOR_CYAN, COLOR_BLACK);
    init_pair(COLOR_WHITE, COLOR_WHITE, COLOR_BLACK);

    occupant_t *p;
    int pmove;
    while ((p = (occupant_t *)heap_remove_min(&h)))
    {
        // certain occupants killed and updated the occupant array as there heap node set to point null so they should allow to move or reinsert.
        if (p->hn)
        {
            if (p->type == PLAYER)
            {

                // as player need to see the dragon position before he make move
                printVisible(d);

                pmove = move_player(d, (player_t *)p, &h);

                if (pmove == -1)
                {
                    heap_delete(&h);
                    move(0, 0);
                    clrtoeol();
                    printw("%s", "You Quit the game :(");
                    refresh();
                    clear();
                    endwin();
                    return;
                }
                else if (pmove == 1)
                {
                    heap_delete(&h);
                    move(0, 0);
                    clrtoeol();
                    printw("%s", "You won the game! Press Q to return to the terminal");
                    refresh();
                    break;
                }

                // after player movement and when still in game
                printVisible(d);
            }
            else
            {
                // dragon_move returns 1 when dragon move killed PC if not 0
                if (dragon_move(d, (dragon_t *)p, &h))
                {
                    heap_delete(&h);
                    printVisible(d);
                    move(0, 0);
                    clrtoeol();
                    printw("%s", "You lost the game! Press Q to return to the terminal");
                    refresh();
                    break;
                }
            }
        }
    }

    // wait until the Q is presses
    int command;
    do
    {
        command = getch();
    } while (command != 'Q');

    clear();
    endwin();

    // changed to vector: free(dungeon.occupants);
    // clean_occupants(d->occupants); beacuse nothing dynamically allocated where the heap node is freed somewhere using heapdelete
    return;
}
