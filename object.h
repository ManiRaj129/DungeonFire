#ifndef OBJECT_H
#define OBJECT_H

#include "dice.h"
#include <string>
#include "objectTypes.h"
#include "descriptions.h"


typedef struct
{
  int x, y;
} point;

class object
{
public:
  std::string name;
  std::string desc;
  object_type_t type;
  int color;
  int hit;
  dice damage;
  int dodge;
  int defence;
  int weight;
  int speed;
  int attribute;
  int value;
  bool artifact;
  int rarity;
  point loc;

  object(const object_description &d)
      : name(d.get_name()),
        desc(d.get_description()),
        type(d.get_type()),
        color(d.get_color()),
        hit(d.get_hit().roll()),
        damage(d.get_damage()),
        dodge(d.get_dodge().roll()),
        defence(d.get_defence().roll()),
        weight(d.get_weight().roll()),
        speed(d.get_speed().roll()),
        attribute(d.get_attribute().roll()),
        value(d.get_value().roll()),
        artifact(d.get_artifact()),
        rarity(d.get_rarity())
  {
  }
};

#endif