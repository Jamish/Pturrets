#include <pebble.h>
#include <math.h>
#include "game.h"
#include "game_object.h"

// Actors
GO_GameObject GO_GameObjects[MAXGAMEOBJECTS];

// Updates the game object based on gravity, velocity, and position.
void GO_GameObject_Update(GO_GameObject go) {
	if (go.active && !go.empty) {
		// Adjust for gravity
		go.velocity.y += go.gravity;
		
		// Adjust position
		go.position.x += go.velocity.x;
		go.position.y += go.velocity.y;
		
		// Adjust for acceleration
		
		// Adjust for braking
		
	}
}

// Updates all the game objects at once
void GO_GameObject_Update_All() {
	// Call every game object's Update and update_handler (additional functionality) function
	for (int i = 0; i < MAXGAMEOBJECTS; i++) {
		GO_GameObject_Update(GO_GameObjects[i]);
		//GO_GameObjects[i].update_handler(&GO_GameObjects[i]);
		
	}
}

// Initialize Game Object to empty
void GO_Init_Empty(GO_GameObject *go, int id) {
	(*go) = (GO_GameObject) {
		id,
		GO_T_BASE,
		{ 0, 0 }, // position
		{ 0, 0 }, // vel
		{ 0, 0 }, // size
		0, // grav
		0, // active
		1 // empty
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
		GO_GameObject go = GO_GameObjects[i];
		if (go.empty) {
			app_log(APP_LOG_LEVEL_DEBUG, __FILE__ , __LINE__ , "GO init #%d", i);
			go.empty = 0;
			go.active = 1;
			return &GO_GameObjects[i];
		}
	}
	
	// Return null if the game is full.
	app_log(APP_LOG_LEVEL_DEBUG, __FILE__ , __LINE__ , "Game is full.");
	return NULL;
}




