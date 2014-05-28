#include <pebble.h>
#include <math.h>
#include "game_object.h"
	
void go_GameObject_Update(go_GameObject go) {
	
	// Adjust for gravity
	go.velocity.y += go.gravity;
	
	// Adjust position
	go.position.x += go.velocity.x;
	go.position.y += go.velocity.y;
	
	// Adjust for acceleration
	
	// Adjust for braking
		
}