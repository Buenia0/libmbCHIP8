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

#ifndef LIBMBCHIP8_H
#define LIBMBCHIP8_H

#include <iostream>
#include <sstream>
#include <memory>
#include <algorithm>
#include <vector>
#include "libmbchip8_api.h"
using namespace std;

namespace chip8
{
    class LIBMBCHIP8_API mbCHIP8Frontend
    {
	public:
	    mbCHIP8Frontend()
	    {

	    }

	    virtual bool init()
	    {
		return true;
	    }

	    virtual void shutdown()
	    {
		return;
	    }

	    virtual vector<uint8_t> loadfile(string filename) = 0;
	    virtual void drawPixels()
	    {
		return;
	    }

	    virtual void playAudio(bool is_active)
	    {
		(void)is_active;
		return;
	    }
    };

    class LIBMBCHIP8_API Chip8Logic
    {
	public:
	    Chip8Logic();
	    ~Chip8Logic();

	    bool loadROM(vector<uint8_t> data);
	    void init();
	    void shutdown();
	    void runInstruction();
	    void debugOutput();
	    bool clockHardware();

	    void keyChanged(int key, bool is_pressed);

	    bool getPixel(int x, int y);

	private:
	    // Returns the value of bit 'bit' of value 'value'
	    // as a boolean (true if set, false if clear)
	    template<typename T>
	    bool testbit(T value, int bit)
	    {
		return ((value >> bit) & 1) ? true : false;
	    }

	    // Determines if value is within the range of
	    // [low, high)
	    template<typename T>
	    bool inRange(T value, int low, int high)
	    {
		int val = int(value);
		return ((val >= low) && (val < high));
	    }

	    void setPixel(int x, int y, bool val);
	    bool isPressed(int key);

	    uint16_t getInstruction();

	    uint8_t getReg(int reg);
	    void setReg(int reg, uint8_t data);

	    vector<uint8_t> rom_data;

	    array<uint8_t, 4096> memory;
	    array<bool, 2048> display;
	    // Keypad
	    array<bool, 16> keypad;
	    // Previous keypad
	    array<bool, 16> prev_keypad;
	    // Timers
	    uint8_t delay_timer = 0;
	    uint8_t sound_timer = 0;

	    // General purpose registers
	    array<uint8_t, 16> registers;
	    // I register
	    uint16_t addr_I = 0;
	    // Program counter
	    uint16_t pc = 0;
	    // Stack pointer
	    uint8_t sp = 0;
	    // Stack
	    array<uint16_t, 16> stack;

	    array<uint8_t, 80> fontset =
	    {
		0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
		0x20, 0x60, 0x20, 0x20, 0x70, // 1
		0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
		0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
		0x90, 0x90, 0xF0, 0x10, 0x10, // 4
		0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
		0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
		0xF0, 0x10, 0x20, 0x40, 0x40, // 7
		0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
		0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
		0xF0, 0x90, 0xF0, 0x90, 0x90, // A
		0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
		0xF0, 0x80, 0x80, 0x80, 0xF0, // C
		0xE0, 0x90, 0x90, 0x90, 0xE0, // D
		0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
		0xF0, 0x80, 0xF0, 0x80, 0x80, // F
	    };

	    bool dump = false;
    };

    class LIBMBCHIP8_API Chip8Core
    {
	public:
	    Chip8Core();
	    ~Chip8Core();

	    void setFrontend(mbCHIP8Frontend *cb);

	    bool init();
	    void shutdown();
	    void runCore();
	    void keyChanged(int key, bool is_pressed);

	    void keyPressed(int key);
	    void keyReleased(int key);

	    bool getPixel(int x, int y);

	    bool loadROM(string filename);
	    bool loadROM(vector<uint8_t> data);

	private:
	    mbCHIP8Frontend *front = NULL;
	    unique_ptr<Chip8Logic> core;

	    int steps = 0;
    };
};


#endif // LIBMBCHIP8_H