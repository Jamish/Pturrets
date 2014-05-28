#include <pebble.h>
#include <math.h>
#include "terrain.h"
#include "utils.h"
	
void terrain_generate(void) {
	app_log(APP_LOG_LEVEL_DEBUG, __FILE__ , __LINE__ , "Generating terrain.");
	
	// Start with all white.
	bmpFill(&terrain_bitmap, terrain_not_color);
	
	// Create a "height map" line. Everything below will be filled in as black.
	const int total_segments = 128;
	int16_t height_map[total_segments];
	
	// Start with a horizontal line
	for (int i = 0; i < total_segments; i++) {
		height_map[i] = 0;
	}
	
	
	srand(time(NULL));
	

	
	int range = terrain_height/2 - 30;
	
	
	for (int i = 0; i < total_segments; i++)
	{
		height_map[i] = 0;
	}
	
	// 2^3 = 8
	// This is the number of times we will divide the line into midpoints.
	for (int p = 0; p < 7; p++) {
		app_log(APP_LOG_LEVEL_DEBUG, __FILE__ , __LINE__ , "Range is %d", range);
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
        range = (int)(range / 1.2F);
		
    }
	

	/*
	for (int i = 0; i < total_segments; i++) {
		app_log(APP_LOG_LEVEL_DEBUG, __FILE__ , __LINE__ , "Terrain point");
		
		app_log(APP_LOG_LEVEL_DEBUG_VERBOSE, __FILE__ , __LINE__ , "Terrain point:  %d ", height_map[i]);
	}
	*/
	
	// Modify the bitmap??
	// Width of terrain slice
	for (int i = 0; i < total_segments; i++) {
		int rect_x = i+8;
		int rect_w = 2;
		int rect_y = terrain_height/2 + height_map[i]; // Height of terrain is the height_map displacement from the middle of the space
		int rect_h = terrain_height - rect_y; //Height stretches to bottom
		
		GRect rect = { { rect_x, rect_y }, {rect_w, rect_h} };
		bmpFillRect(&terrain_bitmap, rect, terrain_color);
	}
	
	//Draw a circle in the bottom corner to test collapsing terrain
	GPoint origin = {64, 128};
	bmpFillCircle(&terrain_bitmap, origin, 25, terrain_not_color);
	
	/*for (int i = 0; i < terrain_height; i++) {
		for (int j = 0; j < terrain_width/2; j++) {
			bmpPutPixel(&terrain_bitmap, i, j, GColorWhite);
		}
	}*/
}

// Shifts every free-standing terrain pixel down by one
// Returns true if the terrain is finished updating.
bool terrain_update(void) {
	bool still_updating = false;
	// For each vertical, one-pixel slice
	for (int i = 0; i < terrain_width; i++) {
		// For each pixel in the slice, bottom up 
		for (int j = terrain_height - 1; j > 0; j--) {
			// If there is a gap and a pixel to above to fill it, fill it and empty the gap in the space above
			if (bmpGetPixel(&terrain_bitmap, i, j) == terrain_not_color && bmpGetPixel(&terrain_bitmap, i, j-1) == terrain_color) {
				bmpPutPixel(&terrain_bitmap, i, j, terrain_color);
				bmpPutPixel(&terrain_bitmap, i, j-1, terrain_not_color); // TODO candidate for speeding up by not shifting every single pixel, but just the top and bottom of each contiguous segment?
				still_updating = true;
			}
		}
	}
	return still_updating;
}

GBitmap* terrain_get_bitmap(void) {
	return &terrain_bitmap;
}