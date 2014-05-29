#ifndef GAME_H_INCLUDED
#define GAME_H_INCLUDED

#define UPDATE_FREQUENCY (1000/60)
#define UPDATE_TIMER_COOKIE 1

#define SCREENW 144
#define SCREENH 168

#define MAXGAMEOBJECTS 64
	
#define GRAVITY 0.15F
	
#define FG_COLOR GColorWhite
#define BG_COLOR GColorBlack

typedef enum Direction Direction;
enum Direction {
    NORTH, SOUTH, EAST, WEST
};
	
void game_deinit(void);
void game_init(void);

#endif