build:
	gcc -Wall -std=c99 ./src/*.c -o renderer \
	-I/opt/homebrew/opt/sdl2/include \
    -L/opt/homebrew/opt/sdl2/lib \
    -lSDL2 \
    -lm

run:
	./renderer

clean:
	rm renderer