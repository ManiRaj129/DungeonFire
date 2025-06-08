#ifndef MOVEMENT_H
#define MOVEMENT_H

#include <stdint.h>
#include "heap.h"
#include "DungeonGen.h"
#include <string>
#include "dice.h"
#include "descriptions.h"
#include "object.h"

#define player_speed 10
#define INTELLIGENT_DRAGON 0x00000001
#define TELEPATH_DRAGON 0x00000002
#define TUNNELING_DRAGON 0x00000004
#define ERRATIC_DRAGON 0x00000008

#define NPC_SMART 0x00000001
#define NPC_TELEPATH 0x00000002
#define NPC_TUNNEL 0x00000004
#define NPC_ERRATIC 0x00000008
#define NPC_PASS_WALL 0x00000010
#define NPC_DESTROY_OBJ 0x00000020
#define NPC_PICKUP_OBJ 0x00000040
#define NPC_UNIQ 0x00000080
#define NPC_BOSS 0x00000100

#define PLAYER 0b01000000

// inclusive bounds
#define rand_range(min, max) ((rand() % (((max) + 1) - (min))) + (min))

#define isInBound(x, y) (x < (DUNGEON_W - 1) && x > 0 && y < (DUNGEON_H - 1) && y > 0)

class occupant_t
{
public:
    // constructor
    occupant_t() {}

    // type---symbol
    occupant_t(int speed, char type, int hitpoints, dice damage) : speed(speed),
                                                                   type(type), hitpoints(hitpoints),
                                                                   damage(damage)
    {
    }

    // destructor
    virtual ~occupant_t() {}
    // hn become null when the occupant get killed

    int speed;

    // symbol does not say much aboult the dragon as earlier where the info encoded in abilities int
    char type;
    int hitpoints;
    dice damage;

    heap_node_t *hn;
    Point pos;
    // serial Number
    int sNo;

    // initially turn= 1000/speed; later appends
    int turn;
};

class dragon_t : public occupant_t
{
public:
    dragon_t(monster_description &monst_des) : occupant_t(monst_des.get_speed().roll(), monst_des.get_symbol(),
                                                          monst_des.get_hitpoints().roll(), monst_des.get_damage()),
                                               targetPos({0, 0}),
                                               name(monst_des.get_name()),
                                               description(monst_des.get_description()),
                                               color(monst_des.get_color()),
                                               rarity(monst_des.get_rarity()),
                                               abilities(monst_des.get_abilities())
    {
    }

    ~dragon_t()
    {
    }
    Point targetPos;

    std::string name, description;
    std::vector<uint32_t> color;

    int rarity;

    // multiple abilities encoded
    int abilities;
};

class player_t : public occupant_t
{
public:
    player_t() : occupant_t(player_speed, PLAYER, 100, dice(0, 1, 4)) {}
    ~player_t()
    {
    }
    std::vector<object> equipped;
    std::vector<object> carry;
};

void initialise_occupants(Dungeon *d);
void clean_occupants(std::vector<occupant_t *> &occupants);
int killOccupant(Point pos, Dungeon *d, heap_t *h);
void setOccupants(Dungeon *d, heap_t *h);

int dragon_move(Dungeon *d, dragon_t *p, heap_t *h);
void start_game(Dungeon *d);
void set_objects(Dungeon *d);

#endif