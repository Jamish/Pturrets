#include <pebble.h>
#include <math.h>
#include "main.h"
#include "game.h"

Window *window;
TextLayer *titleLayer;
TextLayer *scoreLayer;


AppTimer *timer_handle;
Layer *gameLayer;

int button_up_pressed;
int button_down_pressed;


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
    //move_ball();
    layer_mark_dirty(gameLayer);
	
    timer_handle = app_timer_register(UPDATE_FREQUENCY, &handle_timer_timeout, NULL);
}

void game_init(void) {
	app_log(APP_LOG_LEVEL_DEBUG, __FILE__ , __LINE__ , "Game Init");

	//app_log(APP_LOG_LEVEL_DEBUG_VERBOSE, __FILE__ , __LINE__ , "Settings: \nPaddle Size (w/h): %d / %d \nBall Size (w/h): %d / %d \nUpdate Frequency: every %d ms", PADDLE_WIDTH, PADDLE_HEIGHT, BALL_SIZE_WIDTH ,BALL_SIZE_HEIGHT, UPDATE_FREQUENCY);
	
	window = window_create();
	
    window_set_click_config_provider(window, (ClickConfigProvider) config_provider); // Only till the Accelometer-API is released

    window_set_background_color(window, BG_COLOR);
    window_stack_push(window, true /* Animated */);
	
	button_up_pressed=0;
    button_down_pressed=0;
	/*
	level = 1;
	
	// accelometer
	app_log(APP_LOG_LEVEL_DEBUG, __FILE__ , __LINE__ , "accel init");
	accel_data_service_subscribe(0, handle_accel);
	accel_service_set_sampling_rate(ACCEL_SAMPLING_100HZ);
	accel_service_peek(&calibratedAccelData); //Calibrate data to initial position
	
    
    // Title Layer
    titleLayer = text_layer_create(GRect(0,0,144,25));
    text_layer_set_text_alignment(titleLayer, GTextAlignmentCenter);
    text_layer_set_text(titleLayer, "PebblePong");
    text_layer_set_font(titleLayer, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_ORBITRON_BOLD_15)));
    text_layer_set_background_color(titleLayer, BG_COLOR);
    text_layer_set_text_color(titleLayer, FG_COLOR);
    layer_add_child(window_get_root_layer(window), (Layer *)titleLayer);

    //Score Layer
    scoreLayer = text_layer_create(GRect(0,22,144,19));
    text_layer_set_text_alignment(scoreLayer, GTextAlignmentCenter);
    text_layer_set_text(scoreLayer, "0 | 0");
    text_layer_set_font(scoreLayer, fonts_get_system_font (FONT_KEY_GOTHIC_14));
    text_layer_set_background_color(scoreLayer, BG_COLOR);
    text_layer_set_text_color(scoreLayer, FG_COLOR);
    layer_add_child(window_get_root_layer(window), (Layer *)scoreLayer);


	
    // Game Field

    gameLayer = layer_create(GRect(5, 40, 134, 90));
    layer_set_update_proc(gameLayer, draw_game_field);
    layer_add_child(window_get_root_layer(window), (Layer *)gameLayer);

    validBallField = GRect(1,1,(layer_get_bounds(gameLayer).size.w-BALL_SIZE_WIDTH)-1,(layer_get_bounds(gameLayer).size.h-BALL_SIZE_HEIGHT)-1);
    validPaddleField = GRect(1,1,(layer_get_bounds(gameLayer).size.w-PADDLE_WIDTH)-2,(layer_get_bounds(gameLayer).size.h-PADDLE_HEIGHT)-2);

	
    inverter_layer = inverter_layer_create(GRect(0, 0, 144, 168));
    layer_add_child(window_get_root_layer(window), inverter_layer_get_layer(inverter_layer));
	layer_set_hidden(inverter_layer_get_layer(inverter_layer), !option.invert);
	
    // Player 

    init_player(&pl1, WEST, AI);
    init_player(&pl2, EAST, HUMAN);

    // Ball 
    init_ball(&ball);
	*/
    // Setup timer
    timer_handle = app_timer_register(UPDATE_FREQUENCY, &handle_timer_timeout, NULL);
}

void game_deinit(void) {
	app_log(APP_LOG_LEVEL_DEBUG, __FILE__ , __LINE__ , "Game deinit");
	app_timer_cancel(timer_handle);
    //text_layer_destroy(titleLayer);
    //text_layer_destroy(scoreLayer);
	//layer_destroy(gameLayer);
	//inverter_layer_destroy(inverter_layer);
	window_destroy(window);
	//accel_data_service_unsubscribe();
}
