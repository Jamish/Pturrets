#include <pebble.h>
#include <math.h>
#include "terrain.h"
#include "utils.h"
	
void terrain_generate(void) {
	app_log(APP_LOG_LEVEL_DEBUG, __FILE__ , __LINE__ , "Generating terrain.");
	
	// Start with all white.
	bmpFill(&terrain_bitmap, terrain_not_color);
	
	// Create a "height map" line. Everything below will be filled in as black.
	const int total_segments = 64;
	int16_t height_map[total_segments];
	
	// Start with a horizontal line
	for (int i = 0; i < total_segments; i++) {
		height_map[i] = 0;
	}
	
	
	srand(time(NULL));
	

	
	int range = terrain_height/2;
	app_log(APP_LOG_LEVEL_DEBUG, __FILE__ , __LINE__ , "Range is %d", range);
	
	for (int i = 0; i < total_segments; i++)
	{
		height_map[i] = 0;
	}
	
	// 2^3 = 8
	// This is the number of times we will divide the line into midpoints.
	for (int p = 0; p < 10; p++) {
        int segs = pow(p, 2); // Number of segments in this current iteration.
        for (int j = 0; j < segs; j++) {
            int lo = -range;
            int hi = +range;
            int change = lo + (rand() % (hi - lo));
			
			int start = j / segs * total_segments;
			int end = (j+1) / segs * total_segments;
           			
			// Displace the midpoints
			int len = end-start;
			
			int midPt = len/2;
			for (int i = 1; i <= midPt; i++) {
				int ptDisp = change * i/midPt;
				height_map[start+i] += ptDisp;
				height_map[start+len-i] += ptDisp;
			}
        }
        range /= 2;
    }
	

	/*
	for (int i = 0; i < total_segments; i++) {
		app_log(APP_LOG_LEVEL_DEBUG, __FILE__ , __LINE__ , "Terrain point");
		
		app_log(APP_LOG_LEVEL_DEBUG_VERBOSE, __FILE__ , __LINE__ , "Terrain point:  %d ", height_map[i]);
	}
	*/
	
	// Modify the bitmap??
	// Width of terrain slice
	int slice_width = terrain_width / total_segments;
	for (int i = 0; i < total_segments; i++) {
		int rect_x = slice_width * i;
		int rect_w = slice_width;
		int rect_y = terrain_height/2 + height_map[i]; // Height of terrain is the height_map displacement from the middle of the space
		int rect_h = terrain_height - rect_y; //Height stretches to bottom
		
		GRect rect = { { rect_x, rect_y }, {rect_w, rect_h} };
		bmpFillRect(&terrain_bitmap, rect, terrain_color);
	}
	
	/*for (int i = 0; i < terrain_height; i++) {
		for (int j = 0; j < terrain_width/2; j++) {
			bmpPutPixel(&terrain_bitmap, i, j, GColorWhite);
		}
	}*/
}

GBitmap* terrain_get_bitmap(void) {
	return &terrain_bitmap;
}