#ifndef GAME_OBJECT_H_INCLUDED
#define GAME_OBJECT_H_INCLUDED
	
/*	
typedef struct go_PrecisePoint go_PrecisePoint;
typedef struct go_PreciseSize go_PreciseSize;
typedef struct go_PreciseRect go_PreciseRect;*/

typedef struct {
    float x;
    float y;
} go_PrecisePoint;

typedef struct {
    float x;
    float y;
} go_PreciseVelocity;

typedef struct {
	float w;
	float h;
} go_PreciseSize;

typedef struct {
	go_PrecisePoint origin;
	go_PreciseSize size;
} go_PreciseRect;

typedef struct {
	go_PrecisePoint position;
	go_PreciseVelocity velocity;
	go_PreciseSize size;
	float gravity;
} go_GameObject;

// Updates the game object based on gravity, velocity, and position.
void go_GameObject_Update(go_GameObject *go);

#endif