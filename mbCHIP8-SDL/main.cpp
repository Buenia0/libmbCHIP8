#include "SDL2/SDL.h"
#include <iostream>
#include <fstream>
#include <math.h>

#include "../libmbCHIP8/cpu.h"

#include <AL/al.h>
#include <AL/alc.h>
#include <stdio.h>
#include <limits.h>
using namespace std;
#define SECOND 1
#define SAMPLING_HZ 44100
#define BUFFER_LENGTH (SECOND * SAMPLING_HZ)
#define SOUND_HZ 1040
#ifndef M_PI
#define M_PI		3.14159265358979323846
#endif // M_PI

const Uint32 width = 64;
const Uint32 height = 32;
int scale = 12;

uint32_t pixels[2048];

SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;
SDL_Texture *sdlTexture = NULL;

ALCdevice *device;
ALCcontext *context;
ALshort data[BUFFER_LENGTH * 2];
ALuint buffer;
ALuint source;
int i;

bool done;
string apptitle = string("mbCHIP8 v0.1");
const char* title = apptitle.c_str();


bool initSDL()
{
    cout << "Initalzing SDL2..." << endl;

    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        cout << "SDL could not be initalized! SDL_Error: " << SDL_GetError() << endl;
        return false;
    }

    window = SDL_CreateWindow(title, 100, 100, (width * scale), (height * scale), SDL_WINDOW_SHOWN);

    if (window == NULL)
    {
        cout << "Window could not be created! SDL_Error: " << SDL_GetError() << endl;
        return false;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
    SDL_RenderSetLogicalSize(renderer, (width * scale), (height * scale));
    sdlTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, 64, 32);

    cout << "Starting OpenAL..." << endl;

    device = alcOpenDevice(NULL);
    context = alcCreateContext(device, NULL);
    alcMakeContextCurrent(context);
    alGenBuffers(1, &buffer);

    for (i = 0; i < BUFFER_LENGTH; i++)
    {
        data[i * 2] = sin(2 * M_PI * SOUND_HZ * i / BUFFER_LENGTH) * SHRT_MAX;
        data[i * 2 + 1] = -1 * sin(2 * M_PI * SOUND_HZ * i / BUFFER_LENGTH) * SHRT_MAX;
    }
    alBufferData(buffer, AL_FORMAT_STEREO16, data, sizeof(data), BUFFER_LENGTH * 2);
    alGenSources(1, &source);
    alSourcei(source, AL_BUFFER, buffer);
    alSourcei(source, AL_LOOPING, AL_TRUE);

    return true;
}

void stopSDL()
{
    cout << "Stopping OpenAL..." << endl;

    alSourceStop(source);
    alDeleteSources(1, &source);
    alDeleteBuffers(1, &buffer);
    alcMakeContextCurrent(NULL);
    alcDestroyContext(context);
    alcCloseDevice(device);

    cout << "Stopping SDL2..." << endl;

    SDL_DestroyWindow(window);
    SDL_Quit();
}

void renderSDL(unsigned char gfx[])
{
    for (int i = 0; i < 2048; i++)
    {
        uint8_t pixel = gfx[i];
        pixels[i] = (0x00FFFFFF * pixel) | 0xFF000000;
    }
    SDL_UpdateTexture(sdlTexture, NULL, pixels, 64 * sizeof(Uint32));
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, sdlTexture, NULL, NULL);
    SDL_RenderPresent(renderer);
}


bool setupGraphics()
{
    if (!initSDL())
    {
        return false;
    }

    return true;
}

void playAudio(unsigned char sound_timer)
{
    if (sound_timer > 0)
    {
        alSourcePlay(source);
    }
    else
    {
        alSourcePause(source);
    }
}

void UpdateKeyReg(SDL_Event &event, Chip8 &chip)
{
    switch (event.type)
    {
        case SDL_KEYDOWN:
            switch(event.key.keysym.sym)
            {
                case SDLK_0: chip.m_KeyData[0x0] = 1; break;
                case SDLK_1: chip.m_KeyData[0x1] = 1; break;
                case SDLK_2: chip.m_KeyData[0x2] = 1; break;
                case SDLK_3: chip.m_KeyData[0x3] = 1; break;
                case SDLK_4: chip.m_KeyData[0x4] = 1; break;
                case SDLK_5: chip.m_KeyData[0x5] = 1; break;
                case SDLK_6: chip.m_KeyData[0x6] = 1; break;
                case SDLK_7: chip.m_KeyData[0x7] = 1; break;
                case SDLK_8: chip.m_KeyData[0x8] = 1; break;
                case SDLK_9: chip.m_KeyData[0x9] = 1; break;
                case SDLK_a: chip.m_KeyData[0xA] = 1; break;
                case SDLK_b: chip.m_KeyData[0xB] = 1; break;
                case SDLK_c: chip.m_KeyData[0xC] = 1; break;
                case SDLK_d: chip.m_KeyData[0xD] = 1; break;
                case SDLK_e: chip.m_KeyData[0xE] = 1; break;
                case SDLK_f: chip.m_KeyData[0xF] = 1; break;
            }
            break;
        case SDL_KEYUP:
            switch(event.key.keysym.sym)
            {
                case SDLK_0: chip.m_KeyData[0x0] = 0; break;
                case SDLK_1: chip.m_KeyData[0x1] = 0; break;
                case SDLK_2: chip.m_KeyData[0x2] = 0; break;
                case SDLK_3: chip.m_KeyData[0x3] = 0; break;
                case SDLK_4: chip.m_KeyData[0x4] = 0; break;
                case SDLK_5: chip.m_KeyData[0x5] = 0; break;
                case SDLK_6: chip.m_KeyData[0x6] = 0; break;
                case SDLK_7: chip.m_KeyData[0x7] = 0; break;
                case SDLK_8: chip.m_KeyData[0x8] = 0; break;
                case SDLK_9: chip.m_KeyData[0x9] = 0; break;
                case SDLK_a: chip.m_KeyData[0xA] = 0; break;
                case SDLK_b: chip.m_KeyData[0xB] = 0; break;
                case SDLK_c: chip.m_KeyData[0xC] = 0; break;
                case SDLK_d: chip.m_KeyData[0xD] = 0; break;
                case SDLK_e: chip.m_KeyData[0xE] = 0; break;
                case SDLK_f: chip.m_KeyData[0xF] = 0; break;
            }
            break;
        case SDL_QUIT:
            done = true;
            break;
    }
}

int main(int argc, char* argv[])
{
    cout << "mbCHIP8 v0.1 - (C) 2019 Buenia." << endl;
    if (argc < 2)
    {
        cout << "Usage: " << argv[0] << " ROM" << endl;
        return 1;
    }

    string rom_path = argv[1];

    if (!setupGraphics())
    {
        cout << "mbCHIP8 could not be initalized." << endl;
        return 1;
    }

    SDL_Event event;
    Chip8 chip;
    Uint32 lasttick = SDL_GetTicks();
    int opcode_count = 0;
    int cyclespersecond = 10;

    cout << "Initalzing mbCHIP8..." << endl;
    if (!chip.LoadROM(argv[1]))
    {
        return 2;
    }
    while (!done)
    {
        while (SDL_PollEvent(&event))
        {
            UpdateKeyReg(event, chip);
        }

        if (opcode_count < cyclespersecond)
        {
            chip.ExecuteNextOpcode(chip.m_Opcode);
            opcode_count++;
        }

        if (SDL_GetTicks() - lasttick >= 1000/60)
        {
            chip.DecreaseTimers();
            playAudio(chip.m_SoundTimer);
            lasttick = SDL_GetTicks();

            renderSDL(chip.m_ScreenData);

            opcode_count = 0;
        }
    }

    stopSDL();

    return 0;
}
