#include "display.h"


SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;
SDL_Texture* color_buffer_texture = NULL;

uint32_t* color_buffer = NULL;
float* z_buffer = NULL;

int window_width = 800;
int window_height = 600;


bool initialize_window(void){
    if (SDL_Init(SDL_INIT_EVERYTHING) !=0 ) {
        fprintf(stderr, "Error initializing SDL. \n");
        return false;
    }

    // get info about monitor size
    SDL_DisplayMode display_mode;
    SDL_GetCurrentDisplayMode(
        0, // displayindex
        &display_mode
    );
    //set full resolution
    //window_width = display_mode.w;
    //window_height = display_mode.h;

    // make the window
    window = SDL_CreateWindow(
        NULL,
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        window_width,
        window_height,
        SDL_WINDOW_BORDERLESS
    );

    if(!window){
        fprintf(stderr, "Error creating the SDL window. \n");
        return false;
    }
    // render the window
    renderer = SDL_CreateRenderer(window, -1, 0);
    if(!renderer){
        fprintf(stderr, "Error creating the SDL renderer. \n");
        return false;
    }
    //draw in full screen, but at whatever resolution the window is
    //SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);

    return true;
}


void destroy_window(void){
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}


void draw_pixel(int x, int y, uint32_t color){
	if(x >= 0 && x < window_width && y >=0 && y < window_height){
    	color_buffer[(window_width * y) + x] = color;
	}
}

void draw_line(int x1, int y1, int x2, int y2, uint32_t color){
	drawLineDDA(x1, y1, x2, y2, color);
}

// basic DDA algo
void drawLineDDA(int x0, int y0, int x1, int y1, uint32_t color){

    int deltaX = (x1 - x0);
    int deltaY= (y1 - y0);

    int longestSide = abs(deltaX) >= abs(deltaY) ? abs(deltaX) : abs(deltaY);

    float xInc = deltaX / (float)longestSide;
    float yInc = deltaY / (float)longestSide;

    float currentX = x0;
    float currentY = y0;

    for(int i =0; i <= longestSide; i++){
        draw_pixel(round(currentX), round(currentY), color);
        currentX += xInc;
        currentY += yInc;
    }

}

void draw_rect(int x, int y, int w, int h, uint32_t color){
    for(int i=0; i < w; i++){
        for(int j=0; j < h; j++){
            int current_x = x+i;
            int current_y = y+j;
            draw_pixel(current_x, current_y, color);
        }
    }
}

void draw_grid(void){

    // draw dots
    for(int y=0; y < window_height; y+=10){
        for(int x=0; x < window_width; x+=10){
            draw_pixel(x, y, 0xFFFF00FF); //0xFF333333
        }
    }

    //draw grid
    // for(int y=0; y < window_height; y++){
    //     for(int x=0; x < window_width; x++){
    //         if(x % 10 == 0 | y % 10 == 0){
    //             color_buffer[(window_width * y) + x] = 0xFF333333;
    //         }
    //     }
    // }
}


void clear_color_buffer(uint32_t color) {
    for(int y=0; y < window_height; y++){
        for(int x=0; x < window_width; x++){
            draw_pixel(x, y, color);
        }
    }
}

void clear_z_buffer(){
    for(int y=0; y < window_height; y++){
        for(int x=0; x < window_width; x++){
            z_buffer[(window_width * y) + x] = 1.0;
        }
    }
}

void render_color_buffer(void){
    SDL_UpdateTexture(
        color_buffer_texture,
        NULL, //whole texture
        color_buffer,
        (int)window_width * sizeof(uint32_t) //row size of texture (pitch)
    );
    SDL_RenderCopy(renderer, color_buffer_texture, NULL, NULL);

}