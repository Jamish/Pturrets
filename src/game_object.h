#ifndef GAME_OBJECT_H_INCLUDED
#define GAME_OBJECT_H_INCLUDED
#include "game.h"


typedef enum GO_Type GO_Type;
typedef struct GO_PrecisePoint GO_PrecisePoint;
typedef struct GO_PreciseVelocity GO_PreciseVelocity;
typedef struct GO_PreciseSize GO_PreciseSize;
typedef struct GO_PreciseRect GO_PreciseRect;
typedef struct GO_GameObject GO_GameObject;

// Represents all the types of Game Objects. Needed for different behaviors. 
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
	int id;
	GO_Type type;
	GO_PrecisePoint position;
	GO_PreciseVelocity velocity;
	GO_PreciseSize size;
	float gravity;
	bool in_air;
	bool active; // Active game object. Inactive objects will not be updated.
	bool empty; // Empty game object. Used by Game Object Manager
	Layer* layer;
	void* data; // Function pointer which returns a void pointer to the start of the struct (that must be allocated on instantiation) that contains data for classes that derive from GO_GameObject. When reading from this, you must cast it.
	//void(*update_handler)(GO_GameObject *); // Function pointer for additional update stuff
};

// Updates the game object based on gravity, velocity, and position.
void GO_GameObject_Update(GO_GameObject* go);

// Updates all the game objects at once
void GO_GameObject_Update_All();

// Draws the game object
void GO_GameObject_Draw(GO_GameObject* go, GContext* ctx);

// Draws all game objects using the given context if their layer matches the argument layer
void GO_GameObject_Draw_All(Layer* layer, GContext* ctx);

// Initialize Game Object to empty
void GO_Init_Empty(GO_GameObject* go, int id);

// Initialize All Game Objects to empty
void GO_Init_All();

// Destroys a Game Object (well, just marks it inactive)
void GO_Destroy(int id);

// Returns a pointer to an empty Game Object to work with.
GO_GameObject* GO_New();

// Returns the game object
GO_GameObject* GO_Get(int id);






#endif