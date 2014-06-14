#include <pebble.h>
#include <math.h>
#include "main.h"
#include "game.h"
#include "terrain.h"
#include "hud.h"
#include "game_object.h"
#include "obj_player.h"
#include "obj_projectile.h"
	
// Layers
Window *window;
TextLayer *titleLayer;
TextLayer *scoreLayer;

Layer *backgroundLayer;
Layer *spriteLayer;
//InverterLayer *invertLayer;
Layer *hudLayer;

// Misc
AppTimer *timer_handle;


// Controls
bool button_up_pressed = 0;
bool button_up_just_pressed = 0; // Just pressed this frame
bool button_down_pressed = 0;
bool button_down_just_pressed = 0; // Just pressed this frame
bool button_center_pressed = 0;
bool button_center_just_pressed = 0; // Just pressed this frame
bool button_back_just_pressed = 0; // Just pressed this frame

// Step
int step = 0;
int game_state = GS_PLAY_ANGLE;

// Sensitivity. Used so the longer a button is pressed, the more drastic it's effect (for changing angle/power)
int sensitivity_key_repeat = 8; // The amount of frames before a 'press and hold' takes effect
int sensitivity_key_repeat_step; // The step count since key repeat was initiated
int sensitivity_start_step; // The step count the key was pressed, held, and started to repeat

int sensitivity_accel = 1; // Acceleration of amount
int sensitivity_max = 5;
int sensitivity_frames = 12; // The number of frames between each acceleration amount

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

/*
void draw_invert_layer(struct Layer *layer, GContext *ctx) {
	OBJ_Player_Data* go_data = (OBJ_Player_Data*)player->data;
	int32_t crosshair_distance = 30;
	int32_t turret_angle = TRIG_MAX_ANGLE * go_data->angle / 360;
	
	int y = (-cos_lookup(turret_angle) * crosshair_distance / TRIG_MAX_RATIO) + player->position.y;
	int x = (sin_lookup(turret_angle) * crosshair_distance / TRIG_MAX_RATIO) + player->position.x;
	graphics_context_set_stroke_color (ctx, BG_COLOR);
	
	graphics_draw_line(ctx, GPoint(x-4, y), GPoint(x-2, y));
	graphics_draw_line(ctx, GPoint(x+4, y), GPoint(x+2, y));
	graphics_draw_line(ctx, GPoint(x, y-4), GPoint(x, y-2));
	graphics_draw_line(ctx, GPoint(x, y+4), GPoint(x, y+2));
}
*/

void draw_hud_layer(struct Layer *layer, GContext *ctx) {
	//app_log(APP_LOG_LEVEL_DEBUG, __FILE__ , __LINE__ , "HUD layer drawing");
	
	if (game_state == GS_PLAY_ANGLE) {
		HUD_draw_angle(layer, ctx);
	}
	
	if (game_state == GS_PLAY_POWER) {
		HUD_draw_power(layer, ctx);
	}
}


void up_up_handler(ClickRecognizerRef recognizer, Window *window) {
    button_up_pressed = false;
}

void up_down_handler(ClickRecognizerRef recognizer, Window *window) {
	if (button_up_pressed == false) {
		button_up_just_pressed = true;
	}
    button_up_pressed = true;
	sensitivity_key_repeat_step = step;
}

void down_up_handler(ClickRecognizerRef recognizer, Window *window) {
    button_down_pressed = false;
}

void down_down_handler(ClickRecognizerRef recognizer, Window *window) {
	if (button_down_pressed == false) {
		button_down_just_pressed = true;
	}
    button_down_pressed = true;
	sensitivity_key_repeat_step = step;
}

void select_up_handler(ClickRecognizerRef recognizer, Window *window) {
    button_center_pressed = false;
}

void select_down_handler(ClickRecognizerRef recognizer, Window *window) {
	if (button_center_pressed == false) {
		button_center_just_pressed = true;
	}
	button_center_pressed = true;
}

void BackSingleClickHandler(ClickRecognizerRef recognizer, Window *window) {
	button_back_just_pressed = true;
}

void BackLongClickHandler(ClickRecognizerRef recognizer, Window *window) {
	window_stack_pop_all(1);
}


void config_provider(void *context) {
    window_set_click_context(BUTTON_ID_UP, context);
    window_raw_click_subscribe(BUTTON_ID_UP, (ClickHandler)up_down_handler, (ClickHandler)up_up_handler, context);
    window_raw_click_subscribe(BUTTON_ID_DOWN, (ClickHandler)down_down_handler, (ClickHandler)down_up_handler, context);
	window_raw_click_subscribe(BUTTON_ID_SELECT, (ClickHandler)select_down_handler, (ClickHandler)select_up_handler, context);
	
	window_single_click_subscribe(BUTTON_ID_BACK, (ClickHandler)BackSingleClickHandler);
	window_long_click_subscribe(BUTTON_ID_BACK,500,(ClickHandler)BackLongClickHandler,NULL);
}

void handle_accel(AccelData *accel_data, uint32_t num_samples) {
  // do nothing
}


// GAME UPDATE ======================================================
void handle_timer_timeout(void *data) {
	GO_GameObject_Update_All();
	
	OBJ_Player_Data* go_data = (OBJ_Player_Data*)player->data;
	
	
	// Calculate the amount the angle/power should change
	int setting_delta;
	if (sensitivity_key_repeat_step == step) {
		//First press of key. 
		setting_delta = 1;
	}
	else if (step < sensitivity_key_repeat_step + sensitivity_key_repeat) {
		//Wait period. Key repeat hasn't kicked in yet.
		setting_delta = 0;
		
	} else {
		//Wait period over. 
		//Reset the key_repeat_step and start the sensitivity_start_step.
		if (sensitivity_key_repeat_step != 0) {
			sensitivity_start_step = step;
		}
		sensitivity_key_repeat_step = 0;
		
		int steps_elapsed = (step - sensitivity_start_step) / sensitivity_frames; // the number of "step units" passed
		setting_delta = 1 + steps_elapsed; 
		setting_delta = setting_delta > sensitivity_max ? sensitivity_max : setting_delta; // limit to sensitivity_max
	}
	
	if (game_state == GS_PLAY_ANGLE) {
		if (button_back_just_pressed) {
			//Quit game?
			//window_stack_pop_all(1);
		}
		if (button_down_pressed) {
			OBJ_Player_Add_Angle(player, setting_delta);
		}
		if (button_up_pressed) {
			OBJ_Player_Add_Angle(player, -setting_delta);
		}
		if (button_center_just_pressed) {
			game_state = GS_PLAY_POWER;
	    }
	} else if (game_state == GS_PLAY_POWER) {
		if (button_back_just_pressed) {
			game_state = GS_PLAY_ANGLE;
		}
		if (button_down_pressed) {
			OBJ_Player_Add_Power(player, -setting_delta);
		}
		if (button_up_pressed) {
			OBJ_Player_Add_Power(player, setting_delta);
		}
		if (button_center_just_pressed) {
			OBJ_Projectile_Init(spriteLayer, go_data->turret_tip.x, go_data->turret_tip.y, go_data->angle, go_data->power, GO_PT_THREE_SHOT);
	    }
	}
		
	// Dirty the sprite layer each frame to redraw
	layer_mark_dirty(spriteLayer);
	
	// Update the terrain and dirty the backgroundLayer if necessary. (maybe make TerrainLayer later?)
	if (terrain_update()) {
		layer_mark_dirty(backgroundLayer);
	}
    
	
	//Clear out the Just Pressed variables
	button_center_just_pressed = false;
	button_up_just_pressed = false;
	button_down_just_pressed = false;
	button_back_just_pressed = false;
	
	step++;
    timer_handle = app_timer_register(UPDATE_FREQUENCY, &handle_timer_timeout, NULL);
}


// GAME INIT ======================================================
void game_init(void) {
	app_log(APP_LOG_LEVEL_DEBUG, __FILE__ , __LINE__ , "Game Init");
	
	window = window_create();
	
    window_set_click_config_provider(window, (ClickConfigProvider) config_provider);

	window_set_fullscreen(window, true);
    window_set_background_color(window, BG_COLOR);
    window_stack_push(window, false /* Not Animated */);
	
	
	// Title Layer
    //titleLayer = text_layer_create(GRect(0,0,144,25));
    /*text_layer_set_text_alignment(titleLayer, GTextAlignmentCenter);
    text_layer_set_text(titleLayer, "PebblePong");
    text_layer_set_font(titleLayer, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_ORBITRON_BOLD_15)));
    text_layer_set_background_color(titleLayer, BG_COLOR);
    text_layer_set_text_color(titleLayer, FG_COLOR);
    layer_add_child(window_get_root_layer(window), (Layer *)titleLayer);*/
	
	// Background Layer
	backgroundLayer = layer_create(GRect((SCREENW - terrain_width)/2, (SCREENW - terrain_width)/2, terrain_height+2, terrain_width+2)); // draw +2 pixels for borders on four sides.
    layer_set_update_proc(backgroundLayer, draw_background_layer);
    layer_add_child(window_get_root_layer(window), (Layer *)backgroundLayer);
	
	// Sprites Layer - draw it over the terrain viewport (add +1 to x and y to account for the 1 px white border)
	spriteLayer = layer_create(GRect(((SCREENW - terrain_width)/2)+1, ((SCREENW - terrain_width)/2)+1, terrain_height, terrain_width));
	layer_set_update_proc(spriteLayer, draw_sprite_layer);
	layer_add_child(window_get_root_layer(window), (Layer *)spriteLayer);
	
	// Invert layer - draw it over the sprites layer
	//invertLayer = inverter_layer_create(GRect(((SCREENW - terrain_width)/2)+1, ((SCREENW - terrain_width)/2)+1, terrain_height, terrain_width));
	//layer_set_update_proc(inverter_layer_get_layer(invertLayer), draw_invert_layer);
    //layer_add_child(window_get_root_layer(window), inverter_layer_get_layer(invertLayer));
	
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
	HUD_init(player);
	
	app_log(APP_LOG_LEVEL_DEBUG, __FILE__ , __LINE__ , "Player is on layer %p, spriteLayer is %p", player->layer, spriteLayer);

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
	//inverter_layer_destroy(invertLayer);
	
	GO_Deinit_All();
	window_destroy(window);
	//accel_data_service_unsubscribe();
}
