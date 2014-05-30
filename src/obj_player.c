#include <pebble.h>
#include <math.h>
#include "obj_player.h"
	
// Draw the player?
void OBJ_Player_Update(GO_GameObject* go) {
	OBJ_Player_Data* go_data = (OBJ_Player_Data*)go->data;
	
	int32_t turret_length = 8;
	int32_t turret_angle = TRIG_MAX_ANGLE * go_data->angle / 360;
	go_data->turret_tip.y = (-cos_lookup(turret_angle) * turret_length / TRIG_MAX_RATIO) + go->position.y;
	go_data->turret_tip.x = (sin_lookup(turret_angle) * turret_length / TRIG_MAX_RATIO) + go->position.x;
	
}

void OBJ_Player_Draw(GO_GameObject* go, GContext* ctx) {
	OBJ_Player_Data* go_data = (OBJ_Player_Data*)go->data;
	//app_log(APP_LOG_LEVEL_DEBUG, __FILE__ , __LINE__ , "player_draw");
	
	graphics_context_set_stroke_color (ctx, BG_COLOR); 
    graphics_context_set_fill_color (ctx, FG_COLOR);  
	
	// Draw a black turret
	graphics_draw_line(ctx, GPoint(go->position.x, go->position.y), GPoint(go_data->turret_tip.x, go_data->turret_tip.y));
	graphics_draw_line(ctx, GPoint(go->position.x, go->position.y), GPoint(go_data->turret_tip.x, go_data->turret_tip.y+1));
	//graphics_draw_line(ctx, GPoint(go->position.x, go->position.y), GPoint(go_data->turret_tip.x, go_data->turret_tip.y-1));
	graphics_draw_line(ctx, GPoint(go->position.x, go->position.y), GPoint(go_data->turret_tip.x+1, go_data->turret_tip.y));
	//graphics_draw_line(ctx, GPoint(go->position.x, go->position.y), GPoint(go_data->turret_tip.x-1, go_data->turret_tip.y));
	
	// Draw a white circle at the location of the player
	graphics_fill_circle(ctx, GPoint(go->position.x, go->position.y), go->size.w/2);
	
	// Draw a black outline circle at the location of the player
	graphics_draw_circle(ctx, GPoint(go->position.x, go->position.y), go->size.w/2);
	
	// Draw a black rectangle base
	graphics_context_set_fill_color (ctx, BG_COLOR);
	graphics_fill_rect(ctx, GRect(go->position.x - go->size.w/2 - 1, go->position.y + 2, go->size.w + 3, go->size.h), 0, GCornerNone);
	
	
	
}

GO_GameObject* OBJ_Player_Init(Layer* layer) {
	app_log(APP_LOG_LEVEL_DEBUG, __FILE__ , __LINE__ , "OBJ_Projectile_Init");
	GO_GameObject* go = GO_New();
	if (go != NULL) {
		go->type = GO_T_PLAYER;
		go->gravity = GRAVITY;
		go->size.w = 6;
		go->size.h = 6;
		go->position.x = 20;
		go->position.y = 50;
		go->layer = layer;
		
		//Malloc the space for Data
		go->data = (void*)malloc(sizeof(OBJ_Player_Data));
		OBJ_Player_Data* go_data = (OBJ_Player_Data*)go->data;
		go_data->angle = 60;
		go_data->power = 50;
		return go;
	}
	
	return NULL;
}

void OBJ_Player_Destroy(GO_GameObject* go) {
	free(go->data);
}

void OBJ_Player_Add_Angle(GO_GameObject* go, int angle) {
	OBJ_Player_Data* go_data = (OBJ_Player_Data*)go->data;
	angle += go_data->angle;
	angle %= 360;
	while (angle < 0) {
		angle += 360;
	}
	
	go_data->angle = angle;
}