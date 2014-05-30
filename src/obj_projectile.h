#ifndef OBJ_PROJECTILE_H_INCLUDED
#define OBJ_PROJECTILE_H_INCLUDED
#include "game_object.h"	
	
	
typedef struct OBJ_Projectile_Data OBJ_Projectile_Data;
struct OBJ_Projectile_Data {
	int explosion_radius;
	int power;
	int explosion_step;
	bool is_exploding;
};

// Update function
void OBJ_Projectile_Update(GO_GameObject* go);

// Draw function
void OBJ_Projectile_Draw(GO_GameObject* go, GContext* ctx);

// Constructor
GO_GameObject* OBJ_Projectile_Init_Base(Layer* layer);
GO_GameObject* OBJ_Projectile_Init(Layer* layer, float x, float y, int angle, int power);

// Destructor
void OBJ_Projectile_Destroy(GO_GameObject* go);

#endif