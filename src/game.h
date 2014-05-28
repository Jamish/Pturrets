#ifndef GAME_H_INCLUDED
#define GAME_H_INCLUDED

#define UPDATE_FREQUENCY (1000/60)
#define UPDATE_TIMER_COOKIE 1

#define SCREENW 144
#define SCREENH 168
	
#define FG_COLOR GColorBlack
#define BG_COLOR GColorWhite

typedef struct PrecisePoint PrecisePoint;
typedef struct PreciseSize PreciseSize;
typedef struct PreciseRect PreciseRect;

struct PrecisePoint {
    float x;
    float y;
};

struct PreciseSize {
	float w;
	float h;
};




struct PreciseRect {
	PrecisePoint origin;
	PreciseSize size;
};


void game_deinit(void);
void game_init(void);





#endif