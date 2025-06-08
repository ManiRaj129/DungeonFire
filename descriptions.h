#ifndef DESCRIPTIONS_H
#define DESCRIPTIONS_H

#include <stdint.h>
#undef swap
#include <vector>
#include <string>
#include "dice.h"
#include "objectTypes.h"


#define NPC_UNIQ 0x00000080

typedef class Dungeon dungeon_t;

uint32_t parse_descriptions(dungeon_t *d);
uint32_t print_descriptions(dungeon_t *d);
uint32_t destroy_descriptions(dungeon_t *d);

extern const char object_symbol[];

class monster_description
{
private:
  std::string name, description;
  char symbol;
  std::vector<uint32_t> color;
  uint32_t abilities;
  dice speed, hitpoints, damage;
  uint32_t rarity;
  uint32_t num_alive, num_killed;

public:
  monster_description() : name(), description(), symbol(0), color(0),
                          abilities(0), speed(), hitpoints(), damage(),
                          rarity(0), num_alive(0), num_killed(0)
  {
  }
  void set(const std::string &name,
           const std::string &description,
           const char symbol,
           const std::vector<uint32_t> &color,
           const dice &speed,
           const uint32_t abilities,
           const dice &hitpoints,
           const dice &damage,
           const uint32_t rarity);
  std::ostream &print(std::ostream &o);
  char get_symbol() { return symbol; }
  inline const std::string &get_name() const { return name; }
  inline const std::string &get_description() const { return description; }
  inline const std::vector<uint32_t> &get_color() const { return color; }
  inline const dice &get_speed() const { return speed; }
  inline const int get_abilities() const { return abilities; }
  inline const dice &get_hitpoints() const { return hitpoints; }
  inline const dice &get_damage() const { return damage; }
  int get_rarity() const { return rarity; }

  inline void birth()
  {
    num_alive++;
  }
  inline void die()
  {
    num_killed++;
    num_alive--;
  }
 
  inline void resetCount(){
    num_alive =0;
  }

  inline bool can_be_generated()
  {
    return (((abilities & NPC_UNIQ) && !num_alive && !num_killed) ||
            !(abilities & NPC_UNIQ));
  }
  inline bool pass_rarity_roll()
  {
    return rarity > (unsigned)(rand() % 100);
  }

};

class object_description
{
private:
  std::string name, description;
  object_type_t type;
  uint32_t color;
  dice hit, damage, dodge, defence, weight, speed, attribute, value;
  bool artifact;
  uint32_t rarity;


  uint32_t num_generated;

  uint32_t num_found;

public:
  object_description() : name(), description(), type(objtype_no_type),
                         color(0), hit(), damage(),
                         dodge(), defence(), weight(),
                         speed(), attribute(), value(),
                         artifact(false), rarity(0), num_generated(0),
                         num_found(0)
  {
  }

  inline bool can_be_generated()
  {
    return !artifact || (artifact && !num_generated && !num_found);
  }
  inline bool pass_rarity_roll()
  {
    return rarity > (unsigned)(rand() % 100);
  }

  void set(const std::string &name,
           const std::string &description,
           const object_type_t type,
           const uint32_t color,
           const dice &hit,
           const dice &damage,
           const dice &dodge,
           const dice &defence,
           const dice &weight,
           const dice &speed,
           const dice &attribute,
           const dice &value,
           const bool artifact,
           const uint32_t rarity);
  std::ostream &print(std::ostream &o);
  /* Need all these accessors because otherwise there is a *
   * circular dependancy that is difficult to get around.  */
  inline const std::string &get_name() const { return name; }
  inline const std::string &get_description() const { return description; }
  inline const object_type_t get_type() const { return type; }
  inline const uint32_t get_color() const { return color; }
  inline const dice &get_hit() const { return hit; }
  inline const dice &get_damage() const { return damage; }
  inline const dice &get_dodge() const { return dodge; }
  inline const dice &get_defence() const { return defence; }
  inline const dice &get_weight() const { return weight; }
  inline const dice &get_speed() const { return speed; }
  inline const dice &get_attribute() const { return attribute; }
  inline const dice &get_value() const { return value; }
  inline bool get_artifact() const { return artifact; }
  inline const int get_rarity() const { return rarity; }

  inline void generate() { num_generated++; }
  
  inline void resetCount(){num_generated=0;}

  inline void find() { num_found++; }
  
};

std::ostream &operator<<(std::ostream &o, monster_description &m);
std::ostream &operator<<(std::ostream &o, object_description &od);

#endif
