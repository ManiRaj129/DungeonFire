#include "Teleport.h"
#include "PrintUtil.h"
#include <ncurses.h>
#include <stdlib.h>

Point teleportTo(Dungeon *d)
{
    // dungeon is printed from row in ncurses screen
    move(0, 0);
    printw("%s", "Move the Target to desired in bound location.");
    printDungeonNC(d);
    // dungeon is printed from row in ncurses screen
    move(d->PlayerPos.y + 1, d->PlayerPos.x);
    printw("*");
    refresh();

    // default
    int newX = d->PlayerPos.x;
    int newY = d->PlayerPos.y;
    // adjust later based on input
    int x_move = 0;
    int y_move = 0;

    int command = getch();

    while (command != 'g')
    {
        // random
        if (command == 'r')
        {
            do
            {
                newX = 1 + rand() % (DUNGEON_W - 2);
                newY = 1 + rand() % (DUNGEON_H - 2);
            } while (newX == d->PlayerPos.x && newY == d->PlayerPos.y);
            break;
        }
        // Move up-left
        else if (command == '7' || command == 'y')
        {
            y_move -= 1;
            x_move -= 1;
        }
        // Move up
        else if (command == '8' || command == 'k')
        {
            y_move -= 1;
            // x does not change
        }
        // Move up right
        else if (command == '9' || command == 'u')
        {
            y_move -= 1;
            x_move += 1;
        }
        // Move right
        else if (command == '6' || command == 'l')
        {
            // y does not change as same row
            x_move += 1;
        }
        // Move down right
        else if (command == '3' || command == 'n')
        {
            y_move += 1;
            x_move += 1;
        }
        // Move down
        else if (command == '2' || command == 'j')
        {
            y_move += 1;
            // x does not change
        }
        // Move down left
        else if (command == '1' || command == 'b')
        {
            y_move += 1;
            x_move -= 1;
        }
        // Move left
        else if (command == '4' || command == 'h')
        {
            // y does not change
            x_move -= 1;
        }
        // rest
        else if (command == ' ' || command == '5' || command == '.')
        {
            // both x and y does not change
        }
        // invalid
        else
        {
            move(0, 0);
            clrtoeol();
            printw("%s", "invalid command, try again!");
            refresh();
        }

        newX = d->PlayerPos.x + x_move;
        newY = d->PlayerPos.y + y_move;
        if (isInBound(newX, newY))
        {
            printDungeonNC(d);
            move(newY + 1, newX);
            printw("*");
            refresh();
        }
        else
        {
            move(0, 0);
            clrtoeol();
            printw("%s", "invalid move, it moves out of bounds, try again!");
            refresh();
        }
        command = getch();
    }

    Point p = {newX, newY};
    return p;
}
