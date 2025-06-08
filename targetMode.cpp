#include "targetMode.h"
#include "PrintUtil.h"
#include <ncurses.h>
#include <stdlib.h>
#include "Movement.h"

void displayDragon(Point pos, Dungeon *d)
{
    int totalOcuupants = d->DragonNum + 1;

    for (int i = 1; i < totalOcuupants; i++)
    {
        if (pos.x == d->occupants[i]->pos.x && pos.y == d->occupants[i]->pos.y)
        {
            dragon_t *drag = dynamic_cast<dragon_t *>(d->occupants[i]);

            if (drag)
            {
                printw("%s", drag->name.c_str());
                move(2, 0);
                printw("%s", drag->description.c_str());
                break;
            }
        }
    }
}

void targetAt(Dungeon *d)
{
    // dungeon is printed from row in ncurses screen
    move(0, 0);
    clrtoeol();
    printw("%s", "Move the Target on to the any of the visible monster that you wish to know more.");
    printVisible(d);

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

    while (command != 27)
    {

        if (command == 't')
        {

            if (isVisible(newX, newY, d->PlayerPos.x, d->PlayerPos.y))
            {
                if (d->grid[newY][newX].occupant != '*' && d->grid[newY][newX].occupant != '@')
                {
                    clear();
                    move(0, 0);
                    clrtoeol();
                    printw("%s", "press esc to get back to the game!");
                    move(1, 0);
                    Point pos = {newX, newY};
                    displayDragon(pos, d);
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
                    break;
                }
                else
                {
                    move(0, 0);
                    clrtoeol();
                    printw("%s", "select the cell with monster!");
                    refresh();
                }
            }
            else
            {
                move(0, 0);
                clrtoeol();
                printw("%s", "select the cell with in visible radius and with monster!");
                refresh();
            }
        }

        // Move up-left
        if (command == '7' || command == 'y')
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
            printVisible(d);
            move(newY + 1, newX);
            printw("*");
            refresh();
        }
        else
        {
            move(0, 0);
            clrtoeol();
            printw("%s", "invalid move, it moves out of Bounds, try again!");
            refresh();
        }
        command = getch();
    }
}
