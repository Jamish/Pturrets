#include <pebble.h>
#include <math.h>
#include "terrain.h"
#include "obj_projectile.h"
	
void OBJ_Projectile_Update(GO_GameObject* go) {
	OBJ_Projectile_Data* go_data = (OBJ_Projectile_Data*)go->data;
	
	//If the projectile is off the screen, then delete it.
	if (go->position.x < 0 || go->position.y > terrain_width) {
		GO_Destroy(go->id);
	}
	
	//If the projectile is not in the air, then explode
	if (!go->in_air) {
		// Explode terrain
		terrain_destroy_radius(go->position.x, go->position.y, go_data->explosion_radius);
		
		//Animate explosion???
		
		// Destroy self
		GO_Destroy(go->id);
	}
}

void OBJ_Projectile_Draw(GO_GameObject* go, GContext* ctx) {
	//app_log(APP_LOG_LEVEL_DEBUG, __FILE__ , __LINE__ , "player_draw (%d, %d)", (int)go->position.x, (int)go->position.y);
	
	graphics_context_set_stroke_color (ctx, GColorBlack); 
    graphics_context_set_fill_color (ctx, GColorBlack);  
	
	// Draw a circle at the location of the projectile
	graphics_draw_circle(ctx, GPoint(go->position.x, go->position.y), go->size.w/2);
}

GO_GameObject* OBJ_Projectile_Init_Base(Layer* layer) {
	app_log(APP_LOG_LEVEL_DEBUG, __FILE__ , __LINE__ , "OBJ_Projectile_Init");
	GO_GameObject* go = GO_New();
	if (go != NULL) {
		go->type = GO_T_PROJECTILE;
		go->gravity = GRAVITY;
		go->size.w = 2;
		go->size.h = 2;
		go->layer = layer;
		go->in_air = true;
		
		//Malloc the space for Data
		go->data = (void*)malloc(sizeof(OBJ_Projectile_Data));
		OBJ_Projectile_Data* go_data = (OBJ_Projectile_Data*)go->data;
		go_data->explosion_radius = 10;
		go_data->power = 50;
		return go;
	}
	
	return NULL;
}

GO_GameObject* OBJ_Projectile_Init(Layer* layer, float x, float y, int angle, int power) {
	GO_GameObject* go = OBJ_Projectile_Init_Base(layer);
	
	go->position.x = x;
	go->position.y = y;
	
	//Set the velocity x and y
	
	// Calculate imaginary points on the velocity vector with magnitude of 10,000
	GPoint velocity;
	int32_t velocity_magnitude = 10000;
	int32_t trig_angle = TRIG_MAX_ANGLE * angle / 360;
	velocity.y = (-cos_lookup(trig_angle) * velocity_magnitude / TRIG_MAX_RATIO) + go->position.y;
	velocity.x = (sin_lookup(trig_angle) * velocity_magnitude / TRIG_MAX_RATIO) + go->position.x;
	
	// Scale the 100,000 magnitude int velocity down to a smaller, float size
	// A power of 100 would have a velocity of, like, 10?
	int32_t scalar = velocity_magnitude*15;
	go->velocity.x = ((velocity.x - go->position.x) / scalar) * power;
	go->velocity.y = ((velocity.y - go->position.y) / scalar) * power;
	
	return go;
}

void OBJ_Projectile_Destroy(GO_GameObject* go) {
	free(go->data);
}
