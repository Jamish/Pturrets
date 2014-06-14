#include <pebble.h>
#include <math.h>
#include "terrain.h"
#include "utils.h"
	
	
// Some collision checking with terrain
bool terrain_at_position(int x, int y) {
	if (x < 0 || x >= terrain_width) {
		return false; //Never collide with the "walls"
	}
	
	if (y >= terrain_height) {
		return true; //Always collide with the "floor"	
	}
	
	return bmpGetPixel(&terrain_bitmap, x, y) == terrain_color;
}

// Destroys terrain at the given position with radius
void terrain_destroy_radius(int x, int y, int radius) {
	//Draw a circle
	GPoint origin = {x, y};
	bmpFillCircle(&terrain_bitmap, origin, radius, terrain_not_color);
}

void terrain_generate(void) {
	app_log(APP_LOG_LEVEL_DEBUG, __FILE__ , __LINE__ , "Generating terrain.");
	
	// Start with all white.
	bmpFill(&terrain_bitmap, terrain_not_color);
	
	// Create a "height map" line. Everything below will be filled in as black.
	const int total_segments = 128;
	int16_t height_map[total_segments];
	
	// Seed
	srand(time(NULL));
	
	// Starting range value
	int range = terrain_height/2 - 30;
	
	// Start with a horizontal line
	
	int initial_offset = -range + (rand() % (2 * range));	
	
	for (int i = 0; i < total_segments; i++) {
		height_map[i] = (initial_offset/2) + ((i*(initial_offset/2))/total_segments);
	}
	
	// Value of 1 creates varied terrain. Value of 2+ creates smooth terrain. The divisor is what reduces the range for each iteration.
	float range_divisor = 2.0F;
	
	// 2^7 = 128
	// This is the number of times we will divide the line into midpoints.
	for (int p = 0; p < 5; p++) {
		
        int segs = pow(p, 2); // Number of segments in this current iteration.

        for (int j = 0; j < segs; j++) {
            int lo = -range;
            int hi = +range;
            int change = lo + (rand() % (hi - lo));
						
			int start = j * (total_segments / segs);
			int end = (j+1) * (total_segments / segs) + 1;
           			
			// Displace the midpoints
			int len = end-start;
			
			int midPt = len/2;
			for (int i = 1; i <= midPt; i++) {
				int ptDisp = change * i/midPt;
				height_map[start+i] += ptDisp;
				height_map[start+len-i] += ptDisp;
			}
        }
        range = (int)(range / range_divisor);
    }


	// Draw the terrain at the height of each segment
	for (int i = 0; i < total_segments; i++) {
		int rect_x = i;
		int rect_w = 2;
		int rect_y = terrain_height/2 + height_map[i]; // Height of terrain is the height_map displacement from the middle of the space
		int rect_h = terrain_height - rect_y; //Height stretches to bottom
		
		GRect rect = { { rect_x, rect_y }, {rect_w, rect_h} };
		bmpFillRect(&terrain_bitmap, rect, terrain_color);
	}
	
	//Vertical bar to test pixel-by-pixel collision
	//GRect rect2 = { { 64, 0 }, {1, 32} };
	//bmpFillRect(&terrain_bitmap, rect2, terrain_color);
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