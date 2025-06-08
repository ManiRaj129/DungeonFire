#ifndef DUNGEONLOGISTIC_H
#define DUNGEONLOGISTIC_H


void getDirPath(char **dungeon_file_path);
void save(Dungeon *dungeon, const char *dungeon_file_path);
void load(Dungeon *dungeon, const char *dungeon_file_path);


#endif