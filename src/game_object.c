#include <pebble.h>
#include <math.h>
#include "game.h"
#include "game_object.h"

// Actors
GO_GameObject GO_GameObjects[MAXGAMEOBJECTS];

// Updates the game object based on gravity, velocity, and position.
void GO_GameObject_Update(GO_GameObject* go) {
	if (go->active && !go->empty) {
		//app_log(APP_LOG_LEVEL_DEBUG, __FILE__ , __LINE__ , "INFO: P(%d,%d) V(%d,%d) G%d", (int)go->position.x, (int)go->position.y, (int)go->velocity.x, (int)go->velocity.y, (int)go->gravity);
		// Adjust for gravity
		go->velocity.y += go->gravity;
		
		// Adjust position
		go->position.x += go->velocity.x;
		go->position.y += go->velocity.y;
		
		// Adjust for acceleration
		
		// Adjust for braking
		
		// Branch based on type
		switch(go->type) {
			case GO_T_BASE:
				//Do Nothing. Base Type.
				break;
			case GO_T_PLAYER:
				player_update(go);
				break;
			case GO_T_ENEMY:
				
				break;
			case GO_T_PROJECTILE:
				break;
			default:
				break;
			}
	}
}

// Draw the player?
void player_update(GO_GameObject* go) {
	
}

void player_draw(GO_GameObject* go, GContext* ctx) {
	//app_log(APP_LOG_LEVEL_DEBUG, __FILE__ , __LINE__ , "player_draw");
	
	graphics_context_set_stroke_color (ctx, GColorBlack); 
    graphics_context_set_fill_color (ctx, GColorBlack);  
	
	// Draw a circle at the location of the player
	graphics_draw_circle(ctx, GPoint(go->position.x, go->position.y), go->size.w/2);
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
					player_draw(go, ctx);
					break;
				case GO_T_ENEMY:
					
					break;
				case GO_T_PROJECTILE:
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
		0, // active
		1, // empty
		NULL // Layer
	};
}

// Initialize All Game Objects to empty
void GO_Init_All() {
	for (int i = 0; i < MAXGAMEOBJECTS; i++) {
		GO_Init_Empty(&GO_GameObjects[i], i);
	}
}

// Destroys a Game Object (well, just marks it inactive)
void GO_Destroy(int id) {
	for (int i = 0; i < MAXGAMEOBJECTS; i++) {
		GO_GameObjects[i].empty = 1;
		GO_GameObjects[i].active = 0;
	}
}

// Returns a pointer to an empty Game Object to work with.
GO_GameObject* GO_New() {
	for (int i = 0; i < MAXGAMEOBJECTS; i++) {
		GO_GameObject* go = &GO_GameObjects[i];
		if (go->empty) {
			app_log(APP_LOG_LEVEL_DEBUG, __FILE__ , __LINE__ , "GOID #%d init", i);
			go->empty = 0;
			go->active = 1;
			app_log(APP_LOG_LEVEL_DEBUG, __FILE__ , __LINE__ , "GOID #%d is active = %d and empty = %d", i, GO_GameObjects[i].active, GO_GameObjects[i].empty);
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




