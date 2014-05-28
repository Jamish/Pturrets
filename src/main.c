#include <pebble.h>
#include "main.h"
#include "game.h"




static void init(void) {
	app_log(APP_LOG_LEVEL_DEBUG, __FILE__ , __LINE__ , "Init");
	game_init();
}

static void deinit(void) {
	app_log(APP_LOG_LEVEL_DEBUG, __FILE__ , __LINE__ , "denit");
	game_deinit();
}


int main(void) {
    srand(time(NULL));
	app_log(APP_LOG_LEVEL_DEBUG, __FILE__ , __LINE__ , "Start");
	init();
    app_event_loop();
	deinit();
	app_log(APP_LOG_LEVEL_DEBUG, __FILE__ , __LINE__ , "Done");
	return 0;
}
