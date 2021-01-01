All: Compile

Compile:
	gcc main.c -o out `sdl2-config --libs --cflags` -lGL -Wall -lm
