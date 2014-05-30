#include <pebble.h>
#include <math.h>
#include "main.h"
#include "hud.h"
#include "obj_player.h"

	
GO_GameObject *player;
GFont font_wendy_10;
GFont font_wendy_16;

void HUD_init(GO_GameObject *the_player) {
	player = the_player;
	font_wendy_10 = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_WENDY_10));
	font_wendy_16 = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_WENDY_16));
}

void HUD_draw_angle(struct Layer *layer, GContext *ctx) {
	GRect bounds = layer_get_bounds(layer);
    graphics_context_set_stroke_color (ctx, FG_COLOR); 
	graphics_context_set_text_color(ctx, BG_COLOR);    
	
	
	
	// Bounds of the angle meter
	int x1 = bounds.origin.x + 2;
	int y1 = bounds.origin.y + 2;
	int x2 = bounds.origin.x + bounds.size.w - 4;
	int y2 = bounds.origin.y + bounds.size.h - 4;
	
	// Draw a white rectangle inside the layer with 1 px padding
	graphics_context_set_fill_color(ctx, FG_COLOR);
	graphics_fill_rect(ctx, GRect(x1, y1, x2, y2), 0, GCornerNone); 
	
	
	// Draw the angle text of the current angle
	OBJ_Player_Data* go_data = (OBJ_Player_Data*)player->data;
	int angle = go_data->angle;

	// Draw tick marks
	graphics_context_set_stroke_color(ctx, BG_COLOR); 
	int px_per_degree = 3;
	int degrees_per_big_tick = 10;
	int tick_height = 3;
	int width_px = x2 - x1 + 4;
	
	int center_px = width_px / 2; // Example: width of 120 px, center is 60
	int center_angle = angle; // Example: angle of 90
	
	int last_px = width_px;
	int last_angle = (angle + (center_px / px_per_degree))%360;  //90 - (60px / 2 px/deg) = 6
	
	//Convert px to angle
	static char buffer[6] = "";
	for (int i = x1-35; i < width_px+30; i += px_per_degree) {
		//Convert current_px to Angle
		int current_px = i;
		int current_angle = last_angle - (current_px / px_per_degree);

		//If angle is divisible by 10, draw it long.
		if (current_angle % degrees_per_big_tick == 0) {
			
			//Draw the current angle in text
			int draw_angle = current_angle % 360;
			while (draw_angle < 0) {
				draw_angle += 360;
			}
			
			snprintf(
				buffer, 
				sizeof(buffer),
				"%d",
				draw_angle
			);
			
			graphics_draw_text(ctx, buffer, font_wendy_10, GRect(i-5, y1, 15, 15), GTextAlignmentRight, GTextOverflowModeWordWrap, NULL);
			graphics_draw_line(ctx, GPoint(i, y1), GPoint(i, y1+tick_height));
			graphics_draw_line(ctx, GPoint(i, y2+1), GPoint(i, y2+2-tick_height));
		} else {
			//draw it short
			graphics_draw_line(ctx, GPoint(i, y1), GPoint(i, y1+1));
			graphics_draw_line(ctx, GPoint(i, y2+1), GPoint(i, y2+2));
		}
	}
	
	
	// Frame (drawn on top)
	graphics_context_set_stroke_color (ctx, FG_COLOR); 
    graphics_draw_rect(ctx, bounds);
	
	int bar_center_x = bounds.size.w / 2;
	int bar_center_y = bounds.size.h / 2;
	// Draw white circle right in the middle
	graphics_context_set_fill_color(ctx, FG_COLOR);
	graphics_fill_circle(ctx, GPoint(bar_center_x, bar_center_y), 12);
	
	// Draw black circle outline
	graphics_context_set_stroke_color(ctx, BG_COLOR);
	graphics_draw_circle(ctx, GPoint(bar_center_x, bar_center_y), 12);

	
	// Draw current chosen angle right in the middle
	while (angle < 0) {
		angle += 360;
	}
	snprintf(
				buffer, 
				sizeof(buffer),
				"%d",
				angle
			);
	// Offset the characters to center them
	int offset = 0;
	if (angle >= 200) {
		offset = 6;
	} else if (angle >= 100) {
		offset = 5;
	} else if (angle >= 20) {
		offset = 3;
	} else if (angle >= 10) {
		offset = 2;
	} else if (angle == 1) {
		offset = - 1;
	}
	graphics_draw_text(ctx, buffer, font_wendy_16, GRect(bar_center_x - 2 - offset, bar_center_y-11, 18, 18), GTextAlignmentCenter, GTextOverflowModeWordWrap, NULL);
}