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
	
	//If the projectile is not in the air and isn't already exploding, then explode
	if (!go->in_air && !go_data->is_exploding) {
		// Trigger explosion
		go_data->is_exploding = true;
		go_data->explosion_step = 0;
		
		// Disable gravity and velocity
		go->gravity = 0;
		go->velocity.x = 0;
		go->velocity.y = 0;
		
	}
	
	if (go_data->is_exploding) {
		// Explode the terrain at the new size
		terrain_destroy_radius(go->position.x, go->position.y, go_data->explosion_step);
		
		// Increment animation step
		go_data->explosion_step += 1;
		
		// Destroy self if animation is over
		if (go_data->explosion_step >= go_data->explosion_radius) {
			GO_Destroy(go->id);
		}		
	}
}

void OBJ_Projectile_Draw(GO_GameObject* go, GContext* ctx) {
	OBJ_Projectile_Data* go_data = (OBJ_Projectile_Data*)go->data;
	
	// Animate explosion
	if (go_data->is_exploding) {
		graphics_context_set_stroke_color (ctx, BG_COLOR); 
    	graphics_context_set_fill_color (ctx, FG_COLOR);
		// Draw a filled white circle at the explosion's current radius (as the background)
		graphics_fill_circle(ctx, GPoint(go->position.x, go->position.y), go_data->explosion_step);
		//Draw black circles from outside in every 4 pixels
		for (int i = go_data->explosion_step; i >= 0; i-= 4) {
			graphics_draw_circle(ctx, GPoint(go->position.x, go->position.y), i);
		}
	} else {
		// Draw a filled black circle at the location of the projectile if it's not exploding
		switch (go_data->projectile_type) {
			case GO_PT_SMALL_SHOT:
				break;
			case GO_PT_BIG_SHOT:
				break;
			case GO_PT_THREE_SHOT:
				break;
			case GO_PT_NUKE:
				break;
			default:
				break;
		}
		graphics_context_set_stroke_color (ctx, BG_COLOR); 
		graphics_context_set_fill_color (ctx, FG_COLOR);  
		graphics_fill_circle(ctx, GPoint(go->position.x, go->position.y), go->size.w/2);
		graphics_draw_circle(ctx, GPoint(go->position.x, go->position.y), go->size.w/2);
	}
}

GO_GameObject* OBJ_Projectile_Init(Layer* layer, float x, float y, int angle, int power, GO_Projectile_Type type) {
	app_log(APP_LOG_LEVEL_DEBUG, __FILE__ , __LINE__ , "OBJ_Projectile_Init");
	GO_GameObject* go = GO_New();
	if (go != NULL) {
		go->type = GO_T_PROJECTILE;
		go->gravity = GRAVITY;	
		go->layer = layer;
		go->in_air = true;
		
		//Malloc the space for Data
		go->data = (void*)malloc(sizeof(OBJ_Projectile_Data));
		OBJ_Projectile_Data* go_data = (OBJ_Projectile_Data*)go->data;
		go_data->is_exploding = false;
		go_data->projectile_type = type;
		
		
		switch (go_data->projectile_type) {
			case GO_PT_SMALL_SHOT:
				go->size.w = 2;
				go->size.h = 2;
				go_data->explosion_radius = 5;
				go_data->power = 50;
				break;
			case GO_PT_BIG_SHOT:
				go->size.w = 4;
				go->size.h = 4;
				go_data->explosion_radius = 10;
				go_data->power = 30;
				break;
			case GO_PT_THREE_SHOT:
				OBJ_Projectile_Init(layer, x, y, angle-6, power, GO_PT_THREE_SHOT_CHILD);
				OBJ_Projectile_Init(layer, x, y, angle+6, power, GO_PT_THREE_SHOT_CHILD);
			case GO_PT_THREE_SHOT_CHILD:
				go->size.w = 2;
				go->size.h = 2;
				go_data->explosion_radius = 5;
				go_data->power = 20;
			
				break;
			case GO_PT_NUKE:
				go->size.w = 8;
				go->size.h = 8;
				go_data->explosion_radius = 20;
				go_data->power = 80;
				break;
			default:
				break;
		}
	
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
		// A larger scalar means everything is weaker.
		int32_t scalar = velocity_magnitude*20;
		go->velocity.x = ((velocity.x - go->position.x) / scalar) * power;
		go->velocity.y = ((velocity.y - go->position.y) / scalar) * power;
		
		return go;
	}
	
	return NULL;
}

void OBJ_Projectile_Destroy(GO_GameObject* go) {
	free(go->data);
}
