#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <random>
#include "time.h"
#include "cpu.h"
using namespace std;

Chip8::Chip8()
{
}

Chip8::~Chip8()
{
}

void Chip8::ClearScreen()
{
    for (int i = 0; i < 2048; i++)
    {
        m_ScreenData[i] = 0;
    }
}

void Chip8::CPUReset()
{
    m_ProgramCounter = 0x200;
    m_Opcode = 0x0000;
    m_StackPointer = 0;
    m_DelayTimer = 0;
    m_SoundTimer = 0;
    m_AddressI = 0;

    ClearScreen();

    for (int i = 0; i < 16; i++)
    {
        m_KeyData[i] = 0;
    }

    // Clear memory
    for (int i = 0; i < 4096; i++)
    {
        m_GameMemory[i] = 0;
    }

    // Clear registers
    for (int i = 0; i < 16; i++)
    {
        m_Registers[i] = 0;
    }

    // Clear stack
    for (int i = 0; i < 16; i++)
    {
        m_Stack[i] = 0;
    }

    for (int i = 0; i < 80; i++)
    {
        m_GameMemory[i] = m_Fontset[i];
    }

    srand(time(NULL));
}

bool Chip8::LoadROM(const char *filename)
{
    CPUReset();

    printf("Loading ROM: %s\n", filename);

    FILE* rom = fopen(filename, "rb");
    if (rom == NULL)
    {
        printf("Unable to open %s. Please try another ROM.\n", filename);
        return false;
    }

    fseek(rom, 0, SEEK_END);
    long rom_size = ftell(rom);
    rewind(rom);

    char* rom_buffer = (char*) malloc(sizeof(char) * rom_size);
    if (rom_buffer == NULL)
    {
        printf("Unable to allocate memory for %s\n", filename);
        return false;
    }

    size_t result = fread(rom_buffer, sizeof(char), (size_t)rom_size, rom);
    if (result != rom_size)
    {
        printf("Failed to read %s\n", filename);
        return false;
    }

    if ((3584) > rom_size)
    {
        for (int i = 0; i < rom_size; i++)
        {
            m_GameMemory[i + 512] = (uint8_t)rom_buffer[i];
        }
    }
    else
    {
        printf("%s too large to fit in memory", filename);
        return false;
    }

    fclose(rom);
    free(rom_buffer);

    return true;
}

WORD Chip8::GetNextOpcode()
{
    WORD res;
    res = m_GameMemory[m_ProgramCounter];
    res <<= 8;
    res |= m_GameMemory[m_ProgramCounter + 1];
    m_ProgramCounter += 2;
    return res;
}

void Chip8::ExecuteNextOpcode(WORD opcode)
{
    opcode = GetNextOpcode();
    switch (opcode & 0xF000)
    {
        case 0x0000: DecodeOpcode0(opcode); break;
        case 0x1000: Opcode1NNN(opcode); break;
        case 0x2000: Opcode2NNN(opcode); break;
        case 0x3000: Opcode3XKK(opcode); break;
        case 0x4000: Opcode4XKK(opcode); break;
        case 0x5000: Opcode5XY0(opcode); break;
        case 0x6000: Opcode6XKK(opcode); break;
        case 0x7000: Opcode7XKK(opcode); break;
        case 0x8000: DecodeOpcode8(opcode); break;
        case 0x9000: Opcode9XY0(opcode); break;
        case 0xA000: OpcodeANNN(opcode); break;
        case 0xB000: OpcodeBNNN(opcode); break;
        case 0xC000: OpcodeCXKK(opcode); break;
        case 0xD000: OpcodeDXYN(opcode); break;
        case 0xE000: DecodeOpcodeE(opcode); break;
        case 0xF000: DecodeOpcodeF(opcode); break;
        default: printf("Unrecognized opcode at 0x%04x\n", opcode);
    }
}

void Chip8::DecodeOpcode0(WORD opcode)
{
    switch (opcode & 0x00FF)
    {
        case 0x00E0: ClearScreen(); break;
        case 0x00EE: Opcode00EE(); break;
        default: printf("Unrecognized opcode at 0x%04x\n", opcode);
    }
}

void Chip8::DecodeOpcode8(WORD opcode)
{
    switch (opcode & 0x000F)
    {
        case 0x0000: Opcode8XY0(opcode); break;
        case 0x0001: Opcode8XY1(opcode); break;
        case 0x0002: Opcode8XY2(opcode); break;
        case 0x0003: Opcode8XY3(opcode); break;
        case 0x0004: Opcode8XY4(opcode); break;
        case 0x0005: Opcode8XY5(opcode); break;
        case 0x0006: Opcode8XY6(opcode); break;
        case 0x0007: Opcode8XY7(opcode); break;
        case 0x000E: Opcode8XYE(opcode); break;
        default: printf("Unrecognized opcode at 0x%04x\n", opcode);
    }
}

void Chip8::DecodeOpcodeE(WORD opcode)
{
    switch (opcode & 0x00FF)
    {
        case 0x009E: OpcodeEX9E(opcode); break;
        case 0x00A1: OpcodeEXA1(opcode); break;
        default: printf("Unrecognized opcode at 0x%04x\n", opcode);
    }
}

void Chip8::DecodeOpcodeF(WORD opcode)
{
    switch (opcode & 0x00FF)
    {
        case 0x0007: OpcodeFX07(opcode); break;
        case 0x000A: OpcodeFX0A(opcode); break;
        case 0x0015: OpcodeFX15(opcode); break;
        case 0x0018: OpcodeFX18(opcode); break;
        case 0x001E: OpcodeFX1E(opcode); break;
        case 0x0029: OpcodeFX29(opcode); break;
        case 0x0033: OpcodeFX33(opcode); break;
        case 0x0055: OpcodeFX55(opcode); break;
        case 0x0065: OpcodeFX65(opcode); break;
        default: printf("Unrecognized opcode at 0x%04x\n", opcode);
    }
}


void Chip8::Opcode00EE()
{
    m_StackPointer--;
    m_ProgramCounter = m_Stack[m_StackPointer];
}

void Chip8::Opcode1NNN(WORD opcode)
{
    m_ProgramCounter = opcode & 0x0FFF;
}

void Chip8::Opcode2NNN(WORD opcode)
{
    m_Stack[m_StackPointer] = m_ProgramCounter;
    m_StackPointer++;
    m_ProgramCounter = opcode & 0x0FFF;
}

void Chip8::Opcode3XKK(WORD opcode)
{
	if (m_Registers[(opcode & 0x0F00) >> 8] == (opcode & 0x00FF))
	{
        m_ProgramCounter += 2;
	}
}

void Chip8::Opcode4XKK(WORD opcode)
{
    if (m_Registers[(opcode & 0x0F00) >> 8] != (opcode & 0x00FF))
    {
        m_ProgramCounter += 2;
    }
}

void Chip8::Opcode5XY0(WORD opcode)
{
    if (m_Registers[(opcode & 0x0F00) >> 8] == m_Registers[(opcode & 0x00F0) >> 4])
    {
        m_ProgramCounter += 2;
    }
}

void Chip8::Opcode6XKK(WORD opcode)
{
    m_Registers[(opcode & 0x0F00) >> 8] = opcode & 0x00FF;
}

void Chip8::Opcode7XKK(WORD opcode)
{
    m_Registers[(opcode & 0x0F00) >> 8] += opcode & 0x00FF;
}

void Chip8::Opcode8XY0(WORD opcode)
{
    m_Registers[(opcode & 0x0F00) >> 8] = m_Registers[(opcode & 0x00F0) >> 4];
}

void Chip8::Opcode8XY1(WORD opcode)
{
    m_Registers[(opcode & 0x0F00) >> 8] = m_Registers[(opcode & 0x0F00) >> 8] | m_Registers[(opcode & 0x00F0) >> 4];
}

void Chip8::Opcode8XY2(WORD opcode)
{
    m_Registers[(opcode & 0x0F00) >> 8] = m_Registers[(opcode & 0x0F00) >> 8] & m_Registers[(opcode & 0x00F0) >> 4];
}

void Chip8::Opcode8XY3(WORD opcode)
{
    m_Registers[(opcode & 0x0F00) >> 8] = m_Registers[(opcode & 0x0F00) >> 8] ^ m_Registers[(opcode & 0x00F0) >> 4];
}

void Chip8::Opcode8XY4(WORD opcode)
{
    m_Registers[(opcode & 0x0F00) >> 8] += m_Registers[(opcode & 0x00F0) >> 4];
    if (m_Registers[(opcode & 0x00F0) >> 4] > (0xFF - m_Registers[(opcode & 0x0F00) >> 8]))
    {
        m_Registers[0xF] = 1;
    }
    else
    {
        m_Registers[0xF] = 0;
    }
}

void Chip8::Opcode8XY5(WORD opcode)
{
    if (m_Registers[(opcode & 0x00F0) >> 4] > m_Registers[(opcode & 0x0F00) >> 8])
    {
        m_Registers[0xF] = 0;
    }
    else
    {
        m_Registers[0xF] = 1;
    }

    m_Registers[(opcode & 0x0F00) >> 8] -= m_Registers[(opcode & 0x00F0) >> 4];
}

void Chip8::Opcode8XY6(WORD opcode)
{
    m_Registers[0xF] = m_Registers[(opcode & 0x0F00) >> 8] & 0x1;
    m_Registers[(opcode & 0x0F00) >> 8] >>= 1;
}

void Chip8::Opcode8XY7(WORD opcode)
{
    if (m_Registers[(opcode & 0x0F00) >> 8] > m_Registers[(opcode & 0x00F0) >> 4])
    {
        m_Registers[0xF] = 0;
    }
    else
    {
        m_Registers[0xF] = 1;
    }

    m_Registers[(opcode & 0x0F00) >> 8] = m_Registers[(opcode & 0x00F0) >> 4] - m_Registers[(opcode & 0x0F00) >> 8];
}
void Chip8::Opcode8XYE(WORD opcode)
{
    m_Registers[0xF] = (m_Registers[(opcode & 0x0F00) >> 8] & 0x80) >> 7;
    m_Registers[(opcode & 0x0F00) >> 8] <<= 1;
}

void Chip8::Opcode9XY0(WORD opcode)
{
    if (m_Registers[(opcode & 0x0F00) >> 8] != m_Registers[(opcode & 0x00F0) >> 4])
    {
        m_ProgramCounter += 2;
    }
}

void Chip8::OpcodeANNN(WORD opcode)
{
    m_AddressI = opcode & 0x0FFF;
}

void Chip8::OpcodeBNNN(WORD opcode)
{
    m_ProgramCounter = (opcode & 0x0FFF) + m_Registers[0];
}

void Chip8::OpcodeCXKK(WORD opcode)
{
    m_Registers[(opcode & 0x0F00) >> 8] = (rand() % (0xFF + 1)) & (opcode & 0x0FFF);
}

void Chip8::OpcodeDXYN(WORD opcode)
{
    unsigned short coordx = m_Registers[(opcode & 0x0F00) >> 8];
    unsigned short coordy = m_Registers[(opcode & 0x00F0) >> 4];
    unsigned short coordheight = opcode & 0x000F;
    unsigned short pixel;

    m_Registers[0xF] = 0;

    for (int yline = 0; yline < coordheight; yline++)
    {
        pixel = m_GameMemory[m_AddressI + yline];
        for (int xline = 0; xline < 8; xline++)
        {
            if ((pixel & 0x80) > 0)
            {
                if (SetPixel((coordx + xline), (coordy + yline)))
                {
                    m_Registers[0xF] = 1;
                }
            }

	    pixel <<= 1;
        }
    }
}

void Chip8::OpcodeEX9E(WORD opcode)
{
    if (m_KeyData[m_Registers[(opcode & 0x0F00) >> 8]] != 0)
    {
        m_ProgramCounter += 2;
    }
}

void Chip8::OpcodeEXA1(WORD opcode)
{
    if (m_KeyData[m_Registers[(opcode & 0x0F00) >> 8]] == 0)
    {
        m_ProgramCounter += 2;
    }
}

void Chip8::OpcodeFX07(WORD opcode)
{
    m_Registers[(opcode & 0x0F00) >> 8] = m_DelayTimer;
}

void Chip8::OpcodeFX0A(WORD opcode)
{
    bool key_pressed = false;

    for (int i = 0; i < 16; i++)
    {
        if (m_KeyData[i] != 0)
        {
            m_Registers[(opcode & 0x0F00) >> 8] = i;
            key_pressed = true;
        }
    }

    if (!key_pressed)
    {
        return;
    }
}

void Chip8::OpcodeFX15(WORD opcode)
{
    m_DelayTimer = m_Registers[(opcode & 0x0F00) >> 8];
}

void Chip8::OpcodeFX18(WORD opcode)
{
    m_SoundTimer = m_Registers[(opcode & 0x0F00) >> 8];
}

void Chip8::OpcodeFX1E(WORD opcode)
{
    if (m_AddressI + m_Registers[(opcode & 0x0F00) >> 8] > 0xFFF)
    {
        m_Registers[0xF] = 1;
    }
    else
    {
        m_Registers[0xF] = 0;
    }

    m_AddressI += m_Registers[(opcode & 0x0F00) >> 8];
}

void Chip8::OpcodeFX29(WORD opcode)
{
    m_AddressI = m_Registers[(opcode & 0x0F00) >> 8] * 0x5;
}

void Chip8::OpcodeFX33(WORD opcode)
{
    m_GameMemory[m_AddressI] = m_Registers[(opcode & 0x0F00) >> 8] / 100;
    m_GameMemory[m_AddressI + 1] = (m_Registers[(opcode & 0x0F00) >> 8] / 10) % 10;
    m_GameMemory[m_AddressI + 2] = (m_Registers[(opcode & 0x0F00) >> 8] % 100) % 10;
}

void Chip8::OpcodeFX55(WORD opcode)
{
    for (int i = 0; i < ((opcode & 0x0F00) >> 8) + 1; i++)
    {
        m_GameMemory[m_AddressI + i] = m_Registers[i];
    }
}

void Chip8::OpcodeFX65(WORD opcode)
{
    for (int i = 0; i < ((opcode & 0x0F00) >> 8) + 1; i++)
    {
        m_Registers[i] = m_GameMemory[m_AddressI + i];
    }
}

bool Chip8::SetPixel(int x, int y)
{
    if (x > 63)
    {
	while (x > 63)
	{
	    x -= 64;
	}
    }

    if (x < 0)
    {
	while (x < 0)
	{
	    x += 64;
	}
    }

    if (y > 31)
    {
	while (y > 31)
	{
	    y -= 32;
	}
    }

    if (y < 0)
    {
	while (y < 0)
	{
	    y += 32;
	}
    }

    int position = (x + (y * 64));
    m_ScreenData[position] ^= 1;

    return !m_ScreenData[position];
}

void Chip8::DecreaseTimers()
{
    if (m_DelayTimer > 0)
    {
        m_DelayTimer--;
    }
    if (m_SoundTimer > 0)
    {
        m_SoundTimer--;
    }
}
