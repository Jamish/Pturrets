#ifndef GAME_OBJECT_H_INCLUDED
#define GAME_OBJECT_H_INCLUDED
	
typedef struct go_PrecisePoint go_PrecisePoint;
typedef struct go_PreciseVelocity go_PreciseVelocity;
typedef struct go_PreciseSize go_PreciseSize;
typedef struct go_PreciseRect go_PreciseRect;
typedef struct go_GameObject go_GameObject;

struct go_PrecisePoint {
    float x;
    float y;
};

struct go_PreciseVelocity {
    float x;
    float y;
};

struct go_PreciseSize {
	float w;
	float h;
};

struct go_PreciseRect {
	go_PrecisePoint origin;
	go_PreciseSize size;
};

struct go_GameObject {
	go_PrecisePoint position;
	go_PreciseVelocity velocity;
	go_PreciseSize size;
	float gravity;
	// void(*update_handler)(go_GameObject *); // Function pointer for additional update stuff
};

// Updates the game object based on gravity, velocity, and position.
void go_GameObject_Update(go_GameObject go);

#endif