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


void draw_triangle_pixel(int x, int y, uint32_t color, vec4_t point_a, vec4_t point_b, vec4_t point_c){

	vec2_t p = { x, y };
	vec2_t a = vec2_from_vec4(point_a);
	vec2_t b = vec2_from_vec4(point_b);
	vec2_t c = vec2_from_vec4(point_c);
	
	vec3_t weights = barycentric_weights(a, b, c, p);

	float alpha = weights.x;
	float beta = weights.y;
	float gamma = weights.z;

	// need to get reciprocal of the values to account for affine transforms
	float interpolated_reciprocal_w;

	// also interpolate value of 1/w for curren pixel
	interpolated_reciprocal_w = ((1 / point_a.w) * alpha) + ((1 / point_b.w) * beta) + ((1 / point_c.w) * gamma);

	// adjust the recip_w so the pixels that are closer to camera have smaller values
	// so the next check works better
	interpolated_reciprocal_w = 1.0 - interpolated_reciprocal_w;

	// only draw the pixel if the depth value if the depth value is less than what was stored previously stored in the z-buffer
	if(interpolated_reciprocal_w < z_buffer[(window_width * y) + x]){
		draw_pixel(x, y, color);

		// update the z_buffer with the 1/w of this pixel
		z_buffer[(window_width * y) + x] = interpolated_reciprocal_w;
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
void draw_filled_triangle(
	int x0, int y0, float z0, float w0,
	int x1, int y1, float z1, float w1,
	int x2, int y2, float z2, float w2,
	uint32_t color
){
	
	// TODO: use zbuffer

	// we need to sort the verts by the y-coord ascending (y0 < y1 < y2)
	if (y0 > y1) {
        int_swap(&y0, &y1);
        int_swap(&x0, &x1);
        float_swap(&z0, &z1);
        float_swap(&w0, &w1);
    }
    if (y1 > y2) {
        int_swap(&y1, &y2);
        int_swap(&x1, &x2);
        float_swap(&z1, &z2);
        float_swap(&w1, &w2);
    }
    if (y0 > y1) {
        int_swap(&y0, &y1);
        int_swap(&x0, &x1);
        float_swap(&z0, &z1);
        float_swap(&w0, &w1);
    }

    // create vecs and tex coords from the points
    vec4_t point_a = {x0, y0, z0, w0};
    vec4_t point_b = {x1, y1, z1, w1};
    vec4_t point_c = {x2, y2, z2, w2};


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
	    		draw_triangle_pixel(x, y, color, point_a, point_b, point_c);
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
	    		draw_triangle_pixel(x, y, color, point_a, point_b, point_c);
	    	}

	    }
	}


}
// ORIGNINAL METHOD
//void draw_filled_triangle(int x0, int y0, int x1, int y1 ,int x2, int y2, uint32_t color){	
	// // sort the verts by y
	// if (y0 > y1) {
    //     int_swap(&y0, &y1);
    //     int_swap(&x0, &x1);
    // }
    // if (y1 > y2) {
    //     int_swap(&y1, &y2);
    //     int_swap(&x1, &x2);
    // }
    // if (y0 > y1) {
    //     int_swap(&y0, &y1);
    //     int_swap(&x0, &x1);
    // }

	// // since it already has a flat side is flat, we can just render this and end
	// if(y1 == y2){
	// 	// draw flat bottom
	// 	fill_flat_bottom_triangle(x0, y0, x1, y1, x2, y2, color);
	// } else if (y0 == y1){
	// 	// draw flat top
	// 	fill_flat_top_triangle(x0, y0, x1, y1, x2, y2, color);
	// } else {
	// 	// find the mid x using triangle similarity
	// 	int My = y1;
	// 	int Mx = ( (float)((x2 - x0) * (y1 - y0)) / (float)(y2-y0) ) + x0;
		
	// 	// draw flat bottom
	// 	fill_flat_bottom_triangle(x0, y0, x1, y1, Mx, My, color);

	// 	// draw flat top
	// 	fill_flat_top_triangle(x1, y1, Mx, My, x2, y2, color);
	// }
//}


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
	vec4_t point_a, vec4_t point_b, vec4_t point_c,
	tex2_t a_uv, tex2_t b_uv, tex2_t c_uv
){
	vec2_t p = { x, y };
	vec2_t a = vec2_from_vec4(point_a);
	vec2_t b = vec2_from_vec4(point_b);
	vec2_t c = vec2_from_vec4(point_c);
	
	vec3_t weights = barycentric_weights(a, b, c, p);

	float alpha = weights.x;
	float beta = weights.y;
	float gamma = weights.z;

	float interpolated_u;
	float interpolated_v;
	// need to get reciprocal of the values to account for affine transforms
	float interpolated_reciprocal_w;

	// perform interpolation of all U/w and V/w coords using barycentric weights and a factor of 1/w (to fix perpective warping)
	interpolated_u = ((a_uv.u / point_a.w) * alpha) + ((b_uv.u / point_b.w) * beta) + ((c_uv.u / point_c.w) * gamma);
	interpolated_v = ((a_uv.v / point_a.w) * alpha) + ((b_uv.v / point_b.w) * beta) + ((c_uv.v / point_c.w) * gamma);

	// also interpolate value of 1/w for curren pixel
	interpolated_reciprocal_w = ((1 / point_a.w) * alpha) + ((1 / point_b.w) * beta) + ((1 / point_c.w) * gamma);

	// divide back both interpolated values by 1/w to "undo" / change back to linear transform
	interpolated_u /= interpolated_reciprocal_w;
	interpolated_v /= interpolated_reciprocal_w;

	// map the UV coord to the texture w x h
	int tex_x = abs((int)(interpolated_u * texture_width)) % texture_width; // use modulo to ensure it isn't ever outside the range of texture
	int tex_y = abs((int)(interpolated_v * texture_height)) % texture_height;

	// adjust the recip_w so the pixels that are closer to camera have smaller values
	// so the next check works better
	interpolated_reciprocal_w = 1.0 - interpolated_reciprocal_w;

	// only draw the pixel if the depth value if the depth value is less than what was stored previously stored in the z-buffer
	if(interpolated_reciprocal_w < z_buffer[(window_width * y) + x]){
		draw_pixel(x, y, texture[(texture_width * tex_y) + tex_x]);

		// update the z_buffer with the 1/w of this pixel
		z_buffer[(window_width * y) + x] = interpolated_reciprocal_w;
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
	int x0, int y0, float z0, float w0, float u0, float v0,
	int x1, int y1, float z1, float w1, float u1, float v1,
	int x2, int y2, float z2, float w2, float u2, float v2,
	uint32_t* texture
){
	// we need to sort the verts by the y-coord ascending (y0 < y1 < y2)
	if (y0 > y1) {
        int_swap(&y0, &y1);
        int_swap(&x0, &x1);
        float_swap(&z0, &z1);
        float_swap(&w0, &w1);
        float_swap(&u0, &u1);
        float_swap(&v0, &v1);
    }
    if (y1 > y2) {
        int_swap(&y1, &y2);
        int_swap(&x1, &x2);
        float_swap(&z1, &z2);
        float_swap(&w1, &w2);
        float_swap(&u1, &u2);
        float_swap(&v1, &v2);
    }
    if (y0 > y1) {
        int_swap(&y0, &y1);
        int_swap(&x0, &x1);
        float_swap(&z0, &z1);
        float_swap(&w0, &w1);
        float_swap(&u0, &v1);
        float_swap(&u0, &v1);
    }

    // flip the v component to account for inverted UV coords (V grows downwards)
    v0 = 1.0 - v0;
    v1 = 1.0 - v1;
    v2 = 1.0 - v2;

    // create vecs and tex coords from the points
    vec4_t point_a = {x0, y0, z0, w0};
    vec4_t point_b = {x1, y1, z1, w1};
    vec4_t point_c = {x2, y2, z2, w2};
    tex2_t a_uv = { u0, v0 };
    tex2_t b_uv = { u1, v1 };
    tex2_t c_uv = { u2, v2 };

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
	    			a_uv, b_uv, c_uv
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
	    			a_uv, b_uv, c_uv
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


