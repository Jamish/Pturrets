#ifndef GAME_H_INCLUDED
#define GAME_H_INCLUDED

#define UPDATE_FREQUENCY (1000/60)
#define UPDATE_TIMER_COOKIE 1
	

#define FG_COLOR GColorWhite
#define BG_COLOR GColorBlack

typedef struct PrecisePoint PrecisePoint;
typedef struct PreciseSize PreciseSize;
typedef struct PreciseRect PreciseRect;
//typedef enum Side Side;
//typedef enum Direction Direction;
//typedef struct Paddle Paddle;
//typedef enum Pl_type Pl_type;
//typedef struct Player Player;
//typedef struct MovementVector MovementVector;
//typedef struct Ball Ball;

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
/*
enum Side {
    WEST, EAST, SOUTH, NORTH
};

enum Direction {
    LEFT, RIGHT
};

struct Paddle {
    PreciseRect bounds;
};

enum Pl_type {
    HUMAN, AI, REMOTE
};

struct Player {
    uint16_t score;
    Paddle paddle;
    Side side;
    Pl_type pl_type;
    void (*control_handler)(Player *); // Function pointer, which should "controll" the player (e.g. a function that emulates the ki or a function that processes real user input)
	float velocity;
};

struct MovementVector {
    float vx;
    float vy;
};

struct Ball {
    PrecisePoint position;
    GSize size;
    MovementVector vetctor;
	float velocity;
};
*/


void game_deinit(void);
void game_init(void);

#endif