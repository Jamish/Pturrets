#ifndef OBJ_PLAYER_H_INCLUDED
#define OBJ_PLAYER_H_INCLUDED
#include "game_object.h"	
	
	
typedef struct OBJ_Player_Data OBJ_Player_Data;
struct OBJ_Player_Data {
	int angle;
	int power;
	GO_PrecisePoint turret_tip;
};

// Update function
void OBJ_Player_Update(GO_GameObject* go);

// Draw function
void OBJ_Player_Draw(GO_GameObject* go, GContext* ctx);

// Constructor
GO_GameObject* OBJ_Player_Init(Layer* layer);

// Destructor
void OBJ_Player_Destroy(GO_GameObject* go);

void OBJ_Player_Add_Angle(GO_GameObject* go, int angle);

#endif