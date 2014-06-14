#ifndef OBJ_PROJECTILE_H_INCLUDED
#define OBJ_PROJECTILE_H_INCLUDED
#include "game_object.h"	
	
typedef enum GO_Projectile_Type GO_Projectile_Type;

// Represents all the types of Game Objects. Needed for different behaviors. 
enum GO_Projectile_Type {
	GO_PT_SMALL_SHOT,
	GO_PT_BIG_SHOT,
	GO_PT_THREE_SHOT,
	GO_PT_THREE_SHOT_CHILD,
	GO_PT_NUKE
};

typedef struct OBJ_Projectile_Data OBJ_Projectile_Data;
struct OBJ_Projectile_Data {
	GO_Projectile_Type projectile_type;
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
GO_GameObject* OBJ_Projectile_Init(Layer* layer, float x, float y, int angle, int power, GO_Projectile_Type type);

// Destructor
void OBJ_Projectile_Destroy(GO_GameObject* go);

#endif