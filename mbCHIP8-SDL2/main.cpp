/*
    This file is part of libmbCHIP8.
    Copyright (C) 2022 BueniaDev.

    libmbCHIP8 is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    libmbCHIP8 is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with libmbCHIP8.  If not, see <https://www.gnu.org/licenses/>.
*/

// Command-line SDL2 frontend for libmbCHIP8 (used for testing the core logic)

#include <libmbCHIP8/libmbchip8.h>
#include <SDL2/SDL.h>
#include <iostream>
#include <fstream>
#include <chrono>
#include <thread>
#include <cassert>
using namespace chip8;
using namespace std;

class SDL2Frontend : public mbCHIP8Frontend
{
    public:
	SDL2Frontend(Chip8Core &core) : main_core(core)
	{

	}

	~SDL2Frontend()
	{

	}

	bool init()
	{
	    if (SDL_Init(SDL_INIT_VIDEO) < 0)
	    {
		return sdl_error("SDL2 could not be initialized!");
	    }

	    window = SDL_CreateWindow("mbCHIP8-SDL2", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 640, 320, SDL_WINDOW_SHOWN);

	    if (window == NULL)
	    {
		return sdl_error("Window could not be created!");
	    }

	    surface = SDL_CreateRGBSurfaceWithFormat(0, 64, 32, 24, SDL_PIXELFORMAT_RGB888);

	    if (surface == NULL)
	    {
		return sdl_error("Surface could not be created!");
	    }

	    render = SDL_CreateRenderer(window, -1, 0);

	    if (render == NULL)
	    {
		return sdl_error("Renderer could not be created!");
	    }

	    return true;
	}

	void shutdown()
	{
	    if (render != NULL)
	    {
		SDL_DestroyRenderer(render);
		render = NULL;
	    }

	    if (surface != NULL)
	    {
		SDL_FreeSurface(surface);
		surface = NULL;
	    }

	    if (window != NULL)
	    {
		SDL_DestroyWindow(window);
		window = NULL;
	    }

	    SDL_Quit();
	}

	void runcore()
	{
	    while (!quit)
	    {
		runevents();
		run_chip();
	    }
	}

	vector<uint8_t> loadfile(string filename)
	{
	    vector<uint8_t> data;
	    ifstream file(filename, ios::in | ios::binary | ios::ate);

	    if (file.is_open())
	    {
		streampos size = file.tellg();
		data.resize(size, 0);
		file.seekg(0, ios::beg);
		file.read((char*)data.data(), data.size());
		cout << filename << " succesfully loaded." << endl;
		file.close();
	    }

	    return data;
	}

    private:
	bool sdl_error(string message)
	{
	    cout << message << " SDL_Error: " << SDL_GetError() << endl;
	    return false;
	}

	void runevents()
	{
	    while (SDL_PollEvent(&event))
	    {
		switch (event.type)
		{
		    case SDL_QUIT: quit = true; break;
		    case SDL_KEYUP:
		    case SDL_KEYDOWN:
		    {
			bool is_pressed = (event.type == SDL_KEYDOWN);

			switch (event.key.keysym.sym)
			{
			    case SDLK_0: main_core.keyChanged(0, is_pressed); break;
			    case SDLK_1: main_core.keyChanged(1, is_pressed); break;
			    case SDLK_2: main_core.keyChanged(2, is_pressed); break;
			    case SDLK_3: main_core.keyChanged(3, is_pressed); break;
			    case SDLK_4: main_core.keyChanged(4, is_pressed); break;
			    case SDLK_5: main_core.keyChanged(5, is_pressed); break;
			    case SDLK_6: main_core.keyChanged(6, is_pressed); break;
			    case SDLK_7: main_core.keyChanged(7, is_pressed); break;
			    case SDLK_8: main_core.keyChanged(8, is_pressed); break;
			    case SDLK_9: main_core.keyChanged(9, is_pressed); break;
			    case SDLK_a: main_core.keyChanged(10, is_pressed); break;
			    case SDLK_b: main_core.keyChanged(11, is_pressed); break;
			    case SDLK_c: main_core.keyChanged(12, is_pressed); break;
			    case SDLK_d: main_core.keyChanged(13, is_pressed); break;
			    case SDLK_e: main_core.keyChanged(14, is_pressed); break;
			    case SDLK_f: main_core.keyChanged(15, is_pressed); break;
			}
		    }
		    break;
		}
	    }
	}

	void run_chip()
	{
	    main_core.runCore();
	    this_thread::sleep_for(chrono::microseconds(1200));
	}

	void drawPixels()
	{
	    SDL_Rect rect = {0, 0, 1, 1};

	    for (int x = 0; x < 64; x++)
	    {
		rect.x = x;

		for (int y = 0; y < 32; y++)
		{
		    rect.y = y;
		    bool pixel_value = main_core.getPixel(x, y);

		    uint32_t pixel_color = 0;

		    if (pixel_value)
		    {
			pixel_color = SDL_MapRGB(surface->format, 255, 255, 255);
		    }
		    else
		    {
			pixel_color = SDL_MapRGB(surface->format, 0, 0, 0);
		    }

		    SDL_FillRect(surface, &rect, pixel_color);
		}
	    }

	    SDL_Texture *texture = SDL_CreateTextureFromSurface(render, surface);

	    assert(render && texture);
	    SDL_RenderClear(render);
	    SDL_RenderCopy(render, texture, NULL, NULL);
	    SDL_RenderPresent(render);

	    SDL_DestroyTexture(texture);
	}

	Chip8Core &main_core;

	SDL_Window *window = NULL;
	SDL_Surface *surface = NULL;
	SDL_Renderer *render = NULL;

	bool quit = false;
	SDL_Event event;
};

int main(int argc, char* argv[])
{
    if (argc < 2)
    {
	cout << "Usage: mbCHIP8-SDL2 [ROM]" << endl;
	return 1;
    }

    Chip8Core core;

    string rom_name = argv[1];

    SDL2Frontend *front = new SDL2Frontend(core);
    core.setFrontend(front);

    if (!core.loadROM(argv[1]))
    {
	cout << "Could not start mbCHIP8." << endl;
	return 1;
    }

    if (!core.init())
    {
	cout << "Could not start mbCHIP8." << endl;
	return 1;
    }

    front->runcore();
    core.shutdown();
    return 0;
}

