all: chip8 sdlfront

chip8:
	mkdir build
	g++ -c libmbCHIP8/cpu.cpp -o build/cpu.o
	ar -cvq build/libmbCHIP8.a build/cpu.o

sdlfront:
	g++ mbCHIP8-SDL/main.cpp build/libmbCHIP8.a -lSDL2 -lopenal -o mbchip8-sdl

clean: cleanchip8 cleansdl
	

cleanchip8:
	rm -r build/

cleansdl:
	rm mbchip8-sdl
	
