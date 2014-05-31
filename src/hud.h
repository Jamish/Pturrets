#ifndef HUD_H_INCLUDED
#define HUD_H_INCLUDED
#include "game_object.h"
	
void HUD_init(GO_GameObject *the_player);

void HUD_draw_power(struct Layer *layer, GContext *ctx);

void HUD_draw_angle(struct Layer *layer, GContext *ctx);


#endif