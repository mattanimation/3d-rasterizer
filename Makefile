build:
	gcc -Wall -std=c99 ./src/*.c -o renderer \
	-I/opt/homebrew/opt/sdl2/include \
    -L/opt/homebrew/opt/sdl2/lib \
    -lSDL2 \
    -lm

build_linux:
	gcc -Wall -std=c99 ./src/*.c -o renderer \
	-I/usr/include/SDL2 -D_REENTRANT \
	-lSDL2 \
	-lm
# `sdl2-config --cflags --libs` -lm

run:
	./renderer

clean:
	rm renderer