#include "light.h"

light_t light = {
	.direction = { 0, 0, 1 }
};

// used to apply light intensity of a face based on angle
uint32_t light_apply_intensity(uint32_t orignal_color, float percentage_factor){
	
	// just ensure we clamp values from 0-1
	if(percentage_factor < 0) percentage_factor = 0;
	if(percentage_factor > 1) percentage_factor = 1;

	// mask out colors and adjust
	uint32_t a = (orignal_color & 0xFF000000);
	uint32_t r = (orignal_color & 0x00FF0000) * percentage_factor;
	uint32_t g = (orignal_color & 0x0000FF00) * percentage_factor;
	uint32_t b = (orignal_color & 0x000000FF) * percentage_factor;
	// union the masked values after change
	uint32_t new_color = a | (r & 0x00FF0000) | (g & 0x0000FF00) | (b & 0x000000FF);

	return new_color;
}