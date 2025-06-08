#ifndef PROMPT_H

#define MaxEquip 12
#define MaxCarry 10

#include "Movement.h"
#include "object.h"

void carryList(std::vector<object> &c);

void equipList(std::vector<object> &e);
void wear(player_t *p, int i);
int promptCarry(std::vector<object> &c);
int promptEquip(std::vector<object> &e);

#endif