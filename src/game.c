#include <pebble.h>
#include <math.h>
#include "main.h"
#include "game.h"
#include "terrain.h"
#include "game_object.h"
#include "obj_player.h"
#include "obj_projectile.h"
	
// Layers
Window *window;
TextLayer *titleLayer;
TextLayer *scoreLayer;

Layer *backgroundLayer;
Layer *spriteLayer;
Layer *hudLayer;

GFont font_wendy_10;
GFont font_wendy_16;

// Misc
AppTimer *timer_handle;


// Controls
int button_up_pressed;
int button_down_pressed;
int button_center_pressed;

// Step
int step = 0;

// Sensitivity. Used so the longer a button is pressed, the more drastic it's effect (for changing angle/power)
int sensitivity_start_step; // The step count the timer was initiated on
int sensitivity_accel = 1; // Acceleration of amount
int sensitivity_max = 5;
int sensitivity_frames = 12; //  Frames

// Managed Actors that we always want a reference to (no lookup)
GO_GameObject *player;

// TERRAIN REDRAW ======================================================
void draw_background_layer(struct Layer *layer, GContext *ctx) {
    GRect bounds = layer_get_bounds(layer);
    graphics_context_set_stroke_color (ctx, FG_COLOR); 
    graphics_context_set_fill_color (ctx, FG_COLOR);   

    // Frame
    graphics_draw_rect(ctx, bounds);

	// Do Shit
	graphics_draw_bitmap_in_rect(ctx, terrain_get_bitmap(), GRect(1,1,SCREENW,SCREENW));
	
	

    // update score
    /*static char buffer[45] = "";

    snprintf(
        buffer, 
        sizeof(buffer),
        "%d | %d",
        pl1.score, pl2.score
    );
    text_layer_set_text(scoreLayer, buffer);*/
}

void draw_sprite_layer(struct Layer *layer, GContext *ctx) {
	//app_log(APP_LOG_LEVEL_DEBUG, __FILE__ , __LINE__ , "Sprite layer drawing");
	
	GRect bounds = layer_get_bounds(layer);
    graphics_context_set_stroke_color (ctx, BG_COLOR); 
    graphics_context_set_fill_color (ctx, FG_COLOR);   
	graphics_context_set_text_color(ctx, BG_COLOR);
	
	// Draw all the game objects that are in this layer using ctx
	GO_GameObject_Draw_All(layer, ctx);
	

}

void draw_hud_layer(struct Layer *layer, GContext *ctx) {
	//app_log(APP_LOG_LEVEL_DEBUG, __FILE__ , __LINE__ , "HUD layer drawing");
	
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
	/*char c[3];
	int num = angle;
	for(int i=2;i>0&&num>0;i--){
		c[i]=(num%10)+48;
		num/=10;
	}
	graphics_draw_text(ctx, c, font, GRect(x1, y1, x2, y2), GTextAlignmentCenter, GTextOverflowModeWordWrap, NULL);*/
	
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


void up_up_handler(ClickRecognizerRef recognizer, Window *window) {
    button_up_pressed = 0;
}

void up_down_handler(ClickRecognizerRef recognizer, Window *window) {
    button_up_pressed = 1;
	sensitivity_start_step = step;
}

void down_up_handler(ClickRecognizerRef recognizer, Window *window) {
    button_down_pressed = 0;
}

void down_down_handler(ClickRecognizerRef recognizer, Window *window) {
    button_down_pressed = 1;
	sensitivity_start_step = step;
}

void select_up_handler(ClickRecognizerRef recognizer, Window *window) {
    button_center_pressed = 0;
}

void select_down_handler(ClickRecognizerRef recognizer, Window *window) {
	button_center_pressed = 1;
}


void config_provider(void *context) {
    window_set_click_context(BUTTON_ID_UP, context);
    window_raw_click_subscribe(BUTTON_ID_UP, (ClickHandler)up_down_handler, (ClickHandler)up_up_handler, context);
    window_raw_click_subscribe(BUTTON_ID_DOWN, (ClickHandler)down_down_handler, (ClickHandler)down_up_handler, context);
	window_raw_click_subscribe(BUTTON_ID_SELECT, (ClickHandler)select_down_handler, (ClickHandler)select_up_handler, context);
	

}

void handle_accel(AccelData *accel_data, uint32_t num_samples) {
  // do nothing
}


// GAME UPDATE ======================================================
void handle_timer_timeout(void *data) {
	GO_GameObject_Update_All();
	
	OBJ_Player_Data* go_data = (OBJ_Player_Data*)player->data;
	
	// Calculate the amount the angle should change
	int steps_elapsed = (step - sensitivity_start_step) / sensitivity_frames;
	int angle_delta = 1 + steps_elapsed;
	angle_delta = angle_delta > sensitivity_max ? sensitivity_max : angle_delta;
	
	if (button_down_pressed) {
		OBJ_Player_Add_Angle(player, angle_delta);
	}
	
	if (button_up_pressed) {
		OBJ_Player_Add_Angle(player, -angle_delta);
	}
	
	if (button_center_pressed) {
		OBJ_Projectile_Init(spriteLayer, go_data->turret_tip.x, go_data->turret_tip.y, go_data->angle, go_data->power);
	}
	
	// Dirty the sprite layer each frame to redraw
	layer_mark_dirty(spriteLayer);
	
	// Update the terrain and dirty the backgroundLayer if necessary. (maybe make TerrainLayer later?)
	if (terrain_update()) {
		layer_mark_dirty(backgroundLayer);
	}
    
	step++;
    timer_handle = app_timer_register(UPDATE_FREQUENCY, &handle_timer_timeout, NULL);
}


// GAME INIT ======================================================
void game_init(void) {
	app_log(APP_LOG_LEVEL_DEBUG, __FILE__ , __LINE__ , "Game Init");
	
	window = window_create();
	
    window_set_click_config_provider(window, (ClickConfigProvider) config_provider); // Only till the Accelometer-API is released

	window_set_fullscreen(window, true);
    window_set_background_color(window, BG_COLOR);
    window_stack_push(window, false /* Not Animated */);
	
	button_up_pressed=0;
    button_down_pressed=0;
	
	// Title Layer
    //titleLayer = text_layer_create(GRect(0,0,144,25));
    /*text_layer_set_text_alignment(titleLayer, GTextAlignmentCenter);
    text_layer_set_text(titleLayer, "PebblePong");
    text_layer_set_font(titleLayer, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_ORBITRON_BOLD_15)));
    text_layer_set_background_color(titleLayer, BG_COLOR);
    text_layer_set_text_color(titleLayer, FG_COLOR);
    layer_add_child(window_get_root_layer(window), (Layer *)titleLayer);*/
	
	font_wendy_10 = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_WENDY_10));
	font_wendy_16 = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_WENDY_16));
	
	// Background Layer
	backgroundLayer = layer_create(GRect((SCREENW - terrain_width)/2, (SCREENW - terrain_width)/2, terrain_height+2, terrain_width+2)); // draw +2 pixels for borders on four sides.
    layer_set_update_proc(backgroundLayer, draw_background_layer);
    layer_add_child(window_get_root_layer(window), (Layer *)backgroundLayer);
	
	// Sprites Layer - draw it over the terrain viewport (add +1 to x and y to account for the 1 px white border)
	spriteLayer = layer_create(GRect(((SCREENW - terrain_width)/2)+1, ((SCREENW - terrain_width)/2)+1, terrain_height, terrain_width));
	layer_set_update_proc(spriteLayer, draw_sprite_layer);
	layer_add_child(window_get_root_layer(window), (Layer *)spriteLayer);
	
	// HUD Layer
	hudLayer = layer_create(GRect(8, 144, 130, 18)); // draw the layer
	layer_set_update_proc(hudLayer, draw_hud_layer);
	layer_add_child(window_get_root_layer(window), (Layer *)hudLayer);
	
	// Initialize the terrain
	terrain_generate();
	
	// Initialize the actors
	GO_Init_All();
	
	// Create Player
	player = OBJ_Player_Init(spriteLayer);
	
	app_log(APP_LOG_LEVEL_DEBUG, __FILE__ , __LINE__ , "Player is on layer %p, spriteLayer is %p", player->layer, spriteLayer);

	
    //inverter_layer = inverter_layer_create(GRect(0, 0, 144, 168));
    //layer_add_child(window_get_root_layer(window), inverter_layer_get_layer(inverter_layer));
	//layer_set_hidden(inverter_layer_get_layer(inverter_layer), !option.invert);
	
	
	/*
   

    //Score Layer
    scoreLayer = text_layer_create(GRect(0,22,144,19));
    text_layer_set_text_alignment(scoreLayer, GTextAlignmentCenter);
    text_layer_set_text(scoreLayer, "0 | 0");
    text_layer_set_font(scoreLayer, fonts_get_system_font (FONT_KEY_GOTHIC_14));
    text_layer_set_background_color(scoreLayer, BG_COLOR);
    text_layer_set_text_color(scoreLayer, FG_COLOR);
    layer_add_child(window_get_root_layer(window), (Layer *)scoreLayer);
	*/
    // Setup timer
    timer_handle = app_timer_register(UPDATE_FREQUENCY, &handle_timer_timeout, NULL);
}

void game_deinit(void) {
	app_log(APP_LOG_LEVEL_DEBUG, __FILE__ , __LINE__ , "Game deinit");
	app_timer_cancel(timer_handle);
    //text_layer_destroy(titleLayer);
    //text_layer_destroy(scoreLayer);
	layer_destroy(backgroundLayer);
	layer_destroy(spriteLayer);
	layer_destroy(hudLayer);
	//inverter_layer_destroy(inverter_layer);
	window_destroy(window);
	//accel_data_service_unsubscribe();
}
