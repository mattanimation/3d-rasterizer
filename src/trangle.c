#include "triangle.h"
#include "swap.h"

// draw a filled triangle that has a flat bottom, top to bottom
/*
///////////////////////////////////////////////////////////////////////////////
//
//        (x0,y0)
//          / \
//         /   \
//        /     \
//       /       \
//      /         \
//  (x1,y1)------(x2,y2)
//
///////////////////////////////////////////////////////////////////////////////
*/
void fill_flat_bottom_triangle(int x0, int y0, int x1, int y1, int x2, int y2, uint32_t color){

    float inv_slope_1 = (float)(x1 - x0) / (y1 - y0);
    float inv_slope_2 = (float)(x2 - x0) / (y2 - y0);

    // start from the top point
    float x_start  = x0;
    float x_end = x0;

    // loop scan lines from top to bottom
    // start and end grows with the slopes of each legs
    // extending from center point
    for(int y = y0; y <= y2; y++){
    	draw_line(x_start, y, x_end, y, color);
    	x_start += inv_slope_1;
    	x_end += inv_slope_2;
    }

}

// draw a filled triangle with a flat top from bottom to top
/*
///////////////////////////////////////////////////////////////////////////////
//
//  (x0,y0)------(x1,y1)
//      \         /
//       \       /
//        \     /
//         \   /
//          \ /
//        (x2,y2)
//
///////////////////////////////////////////////////////////////////////////////
*/
void fill_flat_top_triangle(int x0, int y0, int x1, int y1, int x2, int y2, uint32_t color){
	
    float inv_slope_1 = (float)(x2 - x0) / (y2 - y0);
    float inv_slope_2 = (float)(x2 - x1) / (y2 - y1);

    // start from the bottom point
    float x_start  = x2;
    float x_end = x2;

    // loop scan lines from bottomt to top
    // start and end grows with the slopes of each legs
    // extending from center point
    for(int y = y2; y >= y0; y--){
    	draw_line(x_start, y, x_end, y, color);
    	x_start -= inv_slope_1;
    	x_end -= inv_slope_2;
    }
}

// scanline based fill
// using flat-bottom/flat-top method
/*
///////////////////////////////////////////////////////////////////////////////
//
//          (x0,y0)
//            / \
//           /   \
//          /     \
//         /       \
//        /         \
//   (x1,y1)------(Mx,My)
//       \_           \
//          \_         \
//             \_       \
//                \_     \
//                   \    \
//                     \_  \
//                        \_\
//                           \
//                         (x2,y2)
//
///////////////////////////////////////////////////////////////////////////////
*/
void draw_filled_triangle(int x0, int y0, int x1, int y1 ,int x2, int y2, uint32_t color){
	
	// sort the verts by y
	if (y0 > y1) {
        int_swap(&y0, &y1);
        int_swap(&x0, &x1);
    }
    if (y1 > y2) {
        int_swap(&y1, &y2);
        int_swap(&x1, &x2);
    }
    if (y0 > y1) {
        int_swap(&y0, &y1);
        int_swap(&x0, &x1);
    }

	// since it already has a flat side is flat, we can just render this and end
	if(y1 == y2){
		// draw flat bottom
		fill_flat_bottom_triangle(x0, y0, x1, y1, x2, y2, color);
	} else if (y0 == y1){
		// draw flat top
		fill_flat_top_triangle(x0, y0, x1, y1, x2, y2, color);
	} else {
		// find the mid x using triangle similarity
		int My = y1;
		int Mx = ( (float)((x2 - x0) * (y1 - y0)) / (float)(y2-y0) ) + x0;
		
		// draw flat bottom
		fill_flat_bottom_triangle(x0, y0, x1, y1, Mx, My, color);

		// draw flat top
		fill_flat_top_triangle(x1, y1, Mx, My, x2, y2, color);
	}
}



///////////////////////////////////////////////////////////////////////////////
// Draw a textured triangle based on a texture array of colors.
// We split the original triangle in two, half flat-bottom and half flat-top.
///////////////////////////////////////////////////////////////////////////////
//
//        v0
//        /\
//       /  \
//      /    \
//     /      \
//   v1--------\
//     \_       \
//        \_     \
//           \_   \
//              \_ \
//                 \\
//                   \
//                    v2
//
///////////////////////////////////////////////////////////////////////////////
void draw_textured_triangle(
	int x0, int y0, float u0, float v0,
	int x1, int y1, float u1, float v1,
	int x2, int y2, float u2, float v2,
	uint32_t* texture
){
	// we need to sort the verts by the y-coord ascending (y0 < y1 < y2)
	if (y0 > y1) {
        int_swap(&y0, &y1);
        int_swap(&x0, &x1);
        float_swap(&u0, &u1);
        float_swap(&v0, &v1);
    }
    if (y1 > y2) {
        int_swap(&y1, &y2);
        int_swap(&x1, &x2);
        float_swap(&u1, &u2);
        float_swap(&v1, &v2);
    }
    if (y0 > y1) {
        int_swap(&y0, &y1);
        int_swap(&x0, &x1);
        float_swap(&u0, &v1);
        float_swap(&u0, &v1);
    }

    ////////////////////////////////////////////////////////
    // Render the upper part of the triangle (flat-bottom)
    ////////////////////////////////////////////////////////

	// left and right legs
    float inv_slope_1 = 0;
    float inv_slope_2 = 0;

    int delta_x1 = x1 - x0;
    int delta_y1 = y1 - y0;
    int delta_x2 = x2 - x0;
    int delta_y2 = y2 - y0;
    // ensure there are no zero division issues and the values are floats
    if(delta_y1 != 0) inv_slope_1 = (float)delta_x1 / abs(delta_y1);
    if(delta_y2 != 0) inv_slope_2 = (float)delta_x2 / abs(delta_y2);

    if(delta_y1 != 0){

	    for(int y = y0; y <= y1; y++){
	    	// find the line start and end for the x scanline
	    	int x_start = x1 + (y - y1) * inv_slope_1;
	    	int x_end = x0 + (y - y0) * inv_slope_2;

	    	// swap if x_start is to the right of x_end
	    	if(x_end < x_start){
	    		int_swap(&x_start, &x_end);
	    	}

	    	for(int x = x_start; x < x_end; x ++){
	    		// use texutre coords to draw pixel
	    		draw_pixel(x,y, (x % 2 == 0) && (y % 2 == 0) ? 0xFFF00FF : 0xFF000000);
	    	}

	    }
	}

	////////////////////////////////////////////////////////
    // Render the bottom part of the triangle (flat-top)
    ////////////////////////////////////////////////////////

	// left and right legs
    inv_slope_1 = 0;
    inv_slope_2 = 0;

    // these change
    delta_x1 = x2 - x1;
    delta_y1 = y2 - y1;
    // these are pretty much same
    delta_x2 = x2 - x0;
    delta_y2 = y2 - y0;
    // ensure there are no zero division issues and the values are floats
    if(delta_y1 != 0) inv_slope_1 = (float)delta_x1 / abs(delta_y1);
    if(delta_y2 != 0) inv_slope_2 = (float)delta_x2 / abs(delta_y2);

    if(delta_y1 != 0){

	    for(int y = y1; y <= y2; y++){
	    	// find the line start and end for the x scanline
	    	int x_start = x1 + (y - y1) * inv_slope_1;
	    	int x_end = x0 + (y - y0) * inv_slope_2;

	    	// swap if x_start is to the right of x_end
	    	if(x_end < x_start){
	    		int_swap(&x_start, &x_end);
	    	}

	    	for(int x = x_start; x < x_end; x ++){
	    		// use texutre coords to draw pixel
	    		draw_pixel(x,y, (x % 2 == 0) && (y % 2 == 0) ? 0xFFF00FF : 0xFF000000);
	    	}

	    }
	}


}


void draw_triangle(int x0, int y0, int x1, int y1 ,int x2, int y2, uint32_t color) {
	draw_line(x0, y0, x1, y1, color);
	draw_line(x1, y1, x2, y2, color);
	draw_line(x2, y2, x0, y0, color);
}
