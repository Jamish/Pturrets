#include <pebble.h>
#include <math.h>
#include "main.h"
#include "game.h"
#include "terrain.h"
#include "game_object.h"

// Layers
Window *window;
TextLayer *titleLayer;
TextLayer *scoreLayer;
Layer *gameLayer;

// Misc
AppTimer *timer_handle;


// Controls
int button_up_pressed;
int button_down_pressed;

// Actors
GO_GameObject game_objects[MAXGAMEOBJECTS];


void draw_game_field(struct Layer *layer, GContext *ctx) {
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


void up_up_handler(ClickRecognizerRef recognizer, Window *window) {
    button_up_pressed=0;
}

void up_down_handler(ClickRecognizerRef recognizer, Window *window) {
    button_up_pressed=1;
}


void down_up_handler(ClickRecognizerRef recognizer, Window *window) {
    button_down_pressed=0;
}

void down_down_handler(ClickRecognizerRef recognizer, Window *window) {
    button_down_pressed=1;
}

void select_down(ClickRecognizerRef recognizer, Window *window) {
}

void select_up(ClickRecognizerRef recognizer, Window *window) {
    
}


void config_provider(void *context) {
    window_set_click_context(BUTTON_ID_UP, context);
    window_raw_click_subscribe(BUTTON_ID_UP, (ClickHandler)up_down_handler, (ClickHandler)up_up_handler, context);
    window_raw_click_subscribe(BUTTON_ID_DOWN, (ClickHandler)down_down_handler, (ClickHandler)down_up_handler, context);
	window_raw_click_subscribe(BUTTON_ID_SELECT, (ClickHandler)select_down, (ClickHandler)select_up, context);
	

}

void handle_accel(AccelData *accel_data, uint32_t num_samples) {
  // do nothing
}

void handle_timer_timeout(void *data) {
	
	// Call every game object's Update and update_handler (additional functionality) function
	for (int i = 0; i < MAXGAMEOBJECTS; i++) {
		GO_GameObject_Update(game_objects[i]);
		//game_objects[i].update_handler(&game_objects[i]);
		
	}

	// Update the terrain and dirty the gameLayer if necessary. (maybe make TerrainLayer later?)
	if (terrain_update()) {
		layer_mark_dirty(gameLayer);
	}
    
    timer_handle = app_timer_register(UPDATE_FREQUENCY, &handle_timer_timeout, NULL);
}



// Initialize Game Object - move to GO_Manager
void initialize_game_object(GO_GameObject *go) {
	(*go) = (GO_GameObject) {
		GO_T_BASE,
		{ 0, 0 }, // position
		{ 0, 0 }, // vel
		{ 0, 0 }, // size
		0, // grav
		0, // active
		1 // empty
	};
}

// Initialize All Game Objects - move to GO_Manager
void initialize_game_objects() {
	for (int i = 0; i < MAXGAMEOBJECTS; i++) {
		initialize_game_object(&game_objects[i]);
	}
}

// Destroys a Game Object (well, just marks it inactive)

// Returns a pointer to an empty Game Object to work with.
GO_GameObject* new_game_object() {
	for (int i = 0; i < MAXGAMEOBJECTS; i++) {
		GO_GameObject go = game_objects[i];
		if (go.empty) {
			app_log(APP_LOG_LEVEL_DEBUG, __FILE__ , __LINE__ , "GO init #%d", i);
			go.empty = 0;
			go.active = 1;
			return &game_objects[i];
		}
	}
	
	// Return null if the game is full.
	app_log(APP_LOG_LEVEL_DEBUG, __FILE__ , __LINE__ , "Game is full.");
	return NULL;
}

GO_GameObject* initialize_player() {
	GO_GameObject* go = new_game_object();
	if (go != NULL) {
		go->type = GO_T_PLAYER;
		go->gravity = 1;
		go->size.w = 3;
		go->size.h = 3;
		
		return go;
	}
	
	return NULL;
}

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
	
	
	// Game Layer
	//gameLayer = layer_create(GRect(0, SCREENH-terrain_height, terrain_height, terrain_width));
	gameLayer = layer_create(GRect((SCREENW - terrain_width)/2, (SCREENW - terrain_width)/2, terrain_height+2, terrain_width+2));
    layer_set_update_proc(gameLayer, draw_game_field);
    layer_add_child(window_get_root_layer(window), (Layer *)gameLayer);
	
	// Initialize the terrain
	terrain_generate();
	
	// Initialize the actors
	initialize_game_objects();
	
	// Create Player
	initialize_player();

    //validBallField = GRect(1,1,(layer_get_bounds(gameLayer).size.w-BALL_SIZE_WIDTH)-1,(layer_get_bounds(gameLayer).size.h-BALL_SIZE_HEIGHT)-1);
    //validPaddleField = GRect(1,1,(layer_get_bounds(gameLayer).size.w-PADDLE_WIDTH)-2,(layer_get_bounds(gameLayer).size.h-PADDLE_HEIGHT)-2);

	
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
	layer_destroy(gameLayer);
	//inverter_layer_destroy(inverter_layer);
	window_destroy(window);
	//accel_data_service_unsubscribe();
}
