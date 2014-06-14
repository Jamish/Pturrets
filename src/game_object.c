#include <pebble.h>
#include <math.h>
#include "game_object.h"
#include "game.h"
#include "terrain.h"
#include "obj_player.h"
#include "obj_projectile.h"

// Actors
GO_GameObject GO_GameObjects[MAXGAMEOBJECTS];

// Updates the game object based on gravity, velocity, and position.
void GO_GameObject_Update(GO_GameObject* go) {
	if (go->active && !go->empty) {
		//app_log(APP_LOG_LEVEL_DEBUG, __FILE__ , __LINE__ , "INFO: P(%d,%d) V(%d,%d) G%d", (int)go->position.x, (int)go->position.y, (int)go->velocity.x, (int)go->velocity.y, (int)go->gravity);
		
		// Check if airborne
		go->in_air = terrain_at_position(go->position.x, go->position.y + 1) ? false : true;

		float x = go->position.x;
		float y = go->position.y;
		float next_x = x + go->velocity.x;
		float next_y = y + go->velocity.y;
		
		// Check for the ground before moving, every pixel.

			
		// The maximum number of pixels to test is just the largest of the two velocities, since we're taking the Manhattan distance
		float xdist = abs(go->velocity.x);
		float ydist = abs(go->velocity.y);
		
		float dist = xdist > ydist ? xdist : ydist;
		
		//Iterate once for the maximum number of pixels.
		float xcheck = x; //xcheck is the current X position to check for collision
		float ycheck = y; //ycheck is the current Y position to check for collision
		
		//Divide the xdist and ydist by dist to get the step factor for each dimension. Each iteration will increment the position by that step factor. The position will be incremented with proper signage.
		float xstep = xdist/dist;
		float ystep = ydist/dist;
		xstep = x < next_x ? xstep : -xstep;
		ystep = y < next_y ? ystep : -ystep;
		for (int i = 0; i <= dist; i++) { // should be <=?
			//Compare
			if (terrain_at_position(xcheck, ycheck)) {
				//Terrain was found. Move to the previous position.
				go->in_air = false;
				next_x = xcheck - xstep;
				next_y = ycheck - ystep;
				break;
			}
			
			//Increment by step
			xcheck += xstep;
			ycheck += ystep;
			
			//If we didn't find terrain in this distance, there is no collision.
		}
		
		// Adjust position
		go->position.x = next_x;
		go->position.y = next_y;
		
		
		
		
		// Branch based on type
		switch(go->type) {
			case GO_T_BASE:
				//Do Nothing. Base Type.
				break;
			case GO_T_PLAYER:
				OBJ_Player_Update(go);
				break;
			case GO_T_ENEMY:
				
				break;
			case GO_T_PROJECTILE:
				OBJ_Projectile_Update(go);
				break;
			default:
				break;
			}
		
		// Adjust for gravity
		if (go->in_air) {
			go->velocity.y += go->gravity;
		} else {
			go->velocity.y = 0;
		}
	}
}

// Updates all the game objects at once
void GO_GameObject_Update_All() {
	// Call every game object's Update and update_handler (additional functionality) function
	for (int i = 0; i < MAXGAMEOBJECTS; i++) {
		GO_GameObject* go = &GO_GameObjects[i];
		GO_GameObject_Update(go);
		
		
		//GO_GameObjects[i].update_handler(&GO_GameObjects[i]);
		
	}
}

// Draws the game object
void GO_GameObject_Draw(GO_GameObject* go, GContext* ctx) {
	//app_log(APP_LOG_LEVEL_DEBUG, __FILE__ , __LINE__ , "GO_GameObject_Draw");
	if (go->active && !go->empty) {
		switch(go->type) {
				case GO_T_BASE:
					//Do Nothing. Base Type.
					break;
				case GO_T_PLAYER:
					OBJ_Player_Draw(go, ctx);
					break;
				case GO_T_ENEMY:
					
					break;
				case GO_T_PROJECTILE:
			  		OBJ_Projectile_Draw(go, ctx);
					break;
				default:
					break;
				}
	}
}

// Draws all game objects using the given context if their layer matches the argument layer
void GO_GameObject_Draw_All(Layer* layer, GContext* ctx) {
	//app_log(APP_LOG_LEVEL_DEBUG, __FILE__ , __LINE__ , "GO_GameObject_Draw_All");
	// Call every game object's Update and update_handler (additional functionality) function
	for (int i = 0; i < MAXGAMEOBJECTS; i++) {
		GO_GameObject* go = &GO_GameObjects[i];
		// Call Draw if the layer matches
		if (layer == go->layer) {
			GO_GameObject_Draw(go, ctx);
		}
	}	
}

// Initialize Game Object to empty
void GO_Init_Empty(GO_GameObject* go, int id) {
	(*go) = (GO_GameObject) {
		id,
		GO_T_BASE,
		{ 0, 0 }, // position
		{ 0, 0 }, // vel
		{ 0, 0 }, // size
		0, // grav
		false, // in air
		false, // active
		true, // empty
		NULL, // Layer
		NULL // Function pointer for data
	};
}

// Initialize All Game Objects to empty
void GO_Init_All() {
	for (int i = 0; i < MAXGAMEOBJECTS; i++) {
		GO_Init_Empty(&GO_GameObjects[i], i);
	}
}

// Destroys a Game Object
void GO_Destroy(int id) {
	if (id < 0 || id > MAXGAMEOBJECTS) {
		app_log(APP_LOG_LEVEL_DEBUG, __FILE__ , __LINE__ , "Deleting GOID #%d out of bounds", id);
		return;
	}
	
	GO_GameObject* go = &GO_GameObjects[id];
	
	if (go->empty) {
		return ;
	}
	
	app_log(APP_LOG_LEVEL_DEBUG, __FILE__ , __LINE__ , "Deleting GOID #%d", id);
	go->empty = 1;
	go->active = 0;
		
	switch(go->type) {
		case GO_T_BASE:
			//Do Nothing. Base Type.
			break;
		case GO_T_PLAYER:
			OBJ_Player_Destroy(go);
			break;
		case GO_T_ENEMY:
			
			break;
		case GO_T_PROJECTILE:
			OBJ_Projectile_Destroy(go);
			break;
		default:
			break;
	}
}


void GO_Deinit_All() {
	for (int i = 0; i < MAXGAMEOBJECTS; i++) {
		GO_GameObject* go = &GO_GameObjects[i];
		if (!go->empty && go->data != NULL) {
			free(go->data);
		}
	}
}

// Returns a pointer to an empty Game Object to work with.
GO_GameObject* GO_New() {
	for (int i = 0; i < MAXGAMEOBJECTS; i++) {
		GO_GameObject* go = &GO_GameObjects[i];
		if (go->empty) {
			app_log(APP_LOG_LEVEL_DEBUG, __FILE__ , __LINE__ , "GOID #%d init", i);
			go->empty = false;
			go->active = true;
			return go;
		}
	}
	
	// Return null if the game is full.
	app_log(APP_LOG_LEVEL_DEBUG, __FILE__ , __LINE__ , "Game is full.");
	return NULL;
}

// Returns the game object
GO_GameObject* GO_Get(int id) {
	if (id < 0 || id > MAXGAMEOBJECTS) {
		app_log(APP_LOG_LEVEL_DEBUG, __FILE__ , __LINE__ , "Requested GOID #%d out of bounds", id);
		return NULL;
	}
	
	GO_GameObject* go = &GO_GameObjects[id];
	
	if (go->empty) {
		app_log(APP_LOG_LEVEL_DEBUG, __FILE__ , __LINE__ , "Requested GOID #%d is empty", id);
		return NULL;
	}

	return go;
}




