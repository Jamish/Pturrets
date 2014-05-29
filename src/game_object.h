#ifndef GAME_OBJECT_H_INCLUDED
#define GAME_OBJECT_H_INCLUDED
	

typedef enum GO_Type GO_Type;
typedef struct GO_PrecisePoint GO_PrecisePoint;
typedef struct GO_PreciseVelocity GO_PreciseVelocity;
typedef struct GO_PreciseSize GO_PreciseSize;
typedef struct GO_PreciseRect GO_PreciseRect;
typedef struct GO_GameObject GO_GameObject;

enum GO_Type {
	GO_T_BASE,
	GO_T_PLAYER,
	GO_T_ENEMY,
	GO_T_PROJECTILE
};

struct GO_PrecisePoint {
    float x;
    float y;
};

struct GO_PreciseVelocity {
    float x;
    float y;
};

struct GO_PreciseSize {
	float w;
	float h;
};

struct GO_PreciseRect {
	GO_PrecisePoint origin;
	GO_PreciseSize size;
};

struct GO_GameObject {
	GO_Type type;
	GO_PrecisePoint position;
	GO_PreciseVelocity velocity;
	GO_PreciseSize size;
	float gravity;
	bool active; // Active game object. Inactive objects will not be updated.
	bool empty; // Empty game object. Used by Game Object Manager
	//void(*update_handler)(GO_GameObject *); // Function pointer for additional update stuff
};

// Updates the game object based on gravity, velocity, and position.
void GO_GameObject_Update(GO_GameObject go);




#endif