Dungeon:DungeonGen.c DungeonLogistic.c Main.c
	gcc -ggdb DungeonGen.c DungeonLogistic.c Main.c -o Dungeon -lm -Wall -Werror

clean: 
	rm -f Dungeon *~ *.o