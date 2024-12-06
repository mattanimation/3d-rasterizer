#include "triangle.h"

void int_swap(int *a, int *b){
	int tmp = *a;
	*a = *b;
	*b = tmp;
}

// draw a filled triangle that has a flat bottom, top to bottom
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