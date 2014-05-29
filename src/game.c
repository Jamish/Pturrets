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
Layer *spriteLayer;
Layer *backgroundLayer;

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
	
	// Draw all the game objects that are in this layer using ctx
	GO_GameObject_Draw_All(layer, ctx);


	// Do Shit
	//graphics_draw_bitmap_in_rect(ctx, terrain_get_bitmap(), GRect(1,1,SCREENW,SCREENW));
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
		go_data->angle += angle_delta;
	}
	
	if (button_up_pressed) {
		go_data->angle -= angle_delta;
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
	
	
	// Background Layer
	backgroundLayer = layer_create(GRect((SCREENW - terrain_width)/2, (SCREENW - terrain_width)/2, terrain_height+2, terrain_width+2)); // draw +2 pixels for borders on four sides.
    layer_set_update_proc(backgroundLayer, draw_background_layer);
    layer_add_child(window_get_root_layer(window), (Layer *)backgroundLayer);
	
	// Sprites Layer - draw it over the terrain viewport (add +1 to x and y to account for the 1 px white border)
	spriteLayer = layer_create(GRect(((SCREENW - terrain_width)/2)+1, ((SCREENW - terrain_width)/2)+1, terrain_height, terrain_width));
	layer_set_update_proc(spriteLayer, draw_sprite_layer);
	layer_add_child(window_get_root_layer(window), (Layer *)spriteLayer);
	
	// Initialize the terrain
	terrain_generate();
	
	// Initialize the actors
	GO_Init_All();
	
	// Create Player
	player = OBJ_Player_Init(spriteLayer);
	
	app_log(APP_LOG_LEVEL_DEBUG, __FILE__ , __LINE__ , "Player is on layer %p, spriteLayer is %p", player->layer, spriteLayer);

	
    //validBallField = GRect(1,1,(layer_get_bounds(backgroundLayer).size.w-BALL_SIZE_WIDTH)-1,(layer_get_bounds(backgroundLayer).size.h-BALL_SIZE_HEIGHT)-1);
    //validPaddleField = GRect(1,1,(layer_get_bounds(backgroundLayer).size.w-PADDLE_WIDTH)-2,(layer_get_bounds(backgroundLayer).size.h-PADDLE_HEIGHT)-2);

	
    //inverter_layer = inverter_layer_create(GRect(0, 0, 144, 168));
    //layer_add_child(window_get_root_layer(window), inverter_layer_get_layer(inverter_layer));
	//layer_set_hidden(inverter_layer_get_layer(inverter_layer), !option.invert);
	
	
	/*
	level = 1;
	
	// accelometer
	app_log(APP_LOG_LEVEL_DEBUG, __FILE__ , __LINE__ , "accel init");
	accel_data_service_subscribe(0, handle_accel);
	accel_service_set_sampling_rate(ACCEL_SAMPLING_100HZ);
	accel_service_peek(&calibratedAccelData); //Calibrate data to initial position
	
    
    

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
	//inverter_layer_destroy(inverter_layer);
	window_destroy(window);
	//accel_data_service_unsubscribe();
}
