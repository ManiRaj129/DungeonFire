#include "prompt.h"
#include <ncurses.h>
#include <stdlib.h>

void carryList(std::vector<object> &c)
{
    clear();
    move(0, 0);
    clrtoeol();
    printw("%s", "Inventory List");

    int i = 0;
    while (i < (int)c.size() && i < MaxCarry)
    {
        move(i + 1, 0);
        clrtoeol();
        printw("%-6d %s", i, c[i].name.c_str());
        i++;
    }
    refresh();
}
void equipList(std::vector<object> &e)
{
    clear();
    move(0, 0);
    clrtoeol();
    printw("%s", "Equipped List");

    char sindex = 'a';
    int i = 0;
    while (i < (int)e.size() && MaxEquip)
    {
        move(i + 1, 0);
        clrtoeol();
        printw("%-6c %s", sindex + i, e[i].name.c_str());
        i++;
    }
    refresh();
}

/*
i : index of object in carry vector
*/
void wear(player_t *p, int i)
{
    // swap when already same equipped
    for (int j = 0; j < (int)p->equipped.size(); j++)
    {
        if (p->equipped[j].type == p->carry[i].type)
        {
            p->carry.push_back(p->equipped[j]);
            p->equipped.erase(p->equipped.begin() + j);
        }
    }

    p->equipped.push_back(p->carry[i]);
    p->carry.erase(p->carry.begin() + i);
}

// return the selected object index or -1 when none slected and escaped
int promptCarry(std::vector<object> &c)
{
    carryList(c);
    move(0, 0);
    clrtoeol();
    printw("%s", "Enter any of the valid slot number from the list");
    refresh();

    int command;
    command = getch();

    int count = (int)c.size();

    while (true)
    {

        if (command == 27)
        {
            return -1;
        }
        else if (command >= '0' || command <= '0' + count)
        {
            // index of the object in vector
            return command - '0';
        }

        move(0, 0);
        clrtoeol();
        printw("%s", "Enter valid command or slot number");
        refresh();
        command = getch();
    }
}

// Return selected object index from vector.
int promptEquip(std::vector<object> &e)
{
    equipList(e);
    move(0, 0);
    clrtoeol();
    printw("%s", "Enter any of the valid slot number from the list");
    refresh();

    int command;
    command = getch();

    int count = (int)e.size();

    while (true)
    {
        if (command == 27)
        {
            return -1;
        }
        else if (command >= 'a' || command <= 'a' + count)
        {
            // index of the object in vector
            return command - 'a';
        }

        move(0, 0);
        clrtoeol();
        printw("%s", "Enter valid command or slot number");
        refresh();
        command = getch();
    }
}