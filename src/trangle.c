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
// Returns the barycentric weights, alpha, beta and gamma for point P
///////////////////////////////////////////////////////////////////////////////
vec3_t barycentric_weights(vec2_t a, vec2_t b, vec2_t c, vec2_t p){
	// find vectors between the verts ABC and point P
	vec2_t ac = vec2_sub(c, a);
	vec2_t ab = vec2_sub(b, a);
	vec2_t pc = vec2_sub(c, p);
	vec2_t pb = vec2_sub(b, p);
	vec2_t ap = vec2_sub(p, a);

	// area of the full parallelogram (tri ABC) use cross product
	float area_pgram_abc = (ac.x * ab.y - ac.y * ab.x);

	// area of the pgram-PBC over the area of full pgram-ABC
	float alpha = (pc.x * pb.y - pc.y * pb.x) / area_pgram_abc;
	// area of the pgram-APC over the area of full pgram-ABC
	float beta = (ac.x * ap.y - ac.y * ap.x) / area_pgram_abc;
	// delta over 1 of other 2 areas to solve for gamma since all values should be 1.0 when added
	float gamma = 1.0 - alpha - beta;

	vec3_t weights = {alpha, beta, gamma};
	return weights;
}


///////////////////////////////////////////////////////////////////////////////
// Function to draw pixel at texture location at pos x and y using interpolation
///////////////////////////////////////////////////////////////////////////////
void draw_texel(
	int x, int y, uint32_t* texture,
	vec2_t point_a, vec2_t point_b, vec2_t point_c,
	float u0, float v0, float u1, float v1, float u2, float v2
){
	vec2_t point_p = { x, y };
	vec3_t weights = barycentric_weights(point_a, point_b, point_c, point_p);
	float alpha = weights.x;
	float beta = weights.y;
	float gamma = weights.z;

	// perform interpolation of all U and V coords using barycentric weights
	float interpolated_u = (u0 * alpha) + (u1 * beta) + (u2 * gamma);
	float interpolated_v = (v0 * alpha) + (v1 * beta) + (v2 * gamma);

	// map the UV coord to the texture w x h
	int tex_x = abs((int)(interpolated_u * texture_width));
	int tex_y = abs((int)(interpolated_v * texture_height));

	draw_pixel(x, y, texture[(texture_width * tex_y) + tex_x]);

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

    // create vecs from the points
    vec2_t point_a = {x0, y0};
    vec2_t point_b = {x1, y1};
    vec2_t point_c = {x2, y2};

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
	    		draw_texel(
	    			x, y, texture,
	    			point_a, point_b, point_c,
	    			u0, v0, u1, v1, u2, v2
	    		);
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
	    		draw_texel(
	    			x, y, texture,
	    			point_a, point_b, point_c,
	    			u0, v0, u1, v1, u2, v2
	    		);
	    	}

	    }
	}


}


void draw_triangle(int x0, int y0, int x1, int y1 ,int x2, int y2, uint32_t color) {
	draw_line(x0, y0, x1, y1, color);
	draw_line(x1, y1, x2, y2, color);
	draw_line(x2, y2, x0, y0, color);
}


