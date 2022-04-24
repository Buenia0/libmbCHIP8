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

#include <libmbchip8.h>
using namespace chip8;
using namespace std;

namespace chip8
{
    Chip8Logic::Chip8Logic()
    {
	srand(time(NULL));
	memory.fill(0);
	registers.fill(0);
	stack.fill(0);
	keypad.fill(false);
	prev_keypad.fill(false);
    }

    Chip8Logic::~Chip8Logic()
    {

    }

    bool Chip8Logic::loadROM(vector<uint8_t> data)
    {
	if (data.empty())
	{
	    cout << "Could not load CHIP-8 ROM." << endl;
	    return false;
	}

	copy(data.begin(), data.end(), (memory.begin() + 0x200));
	return true;
    }

    void Chip8Logic::init()
    {
	// Copy fontset into memory
	copy(fontset.begin(), fontset.end(), memory.begin());

	// Initialize program counter
	pc = 0x200;
    }

    void Chip8Logic::shutdown()
    {
	rom_data.clear();
    }

    void Chip8Logic::keyChanged(int key, bool is_pressed)
    {
	// Throw exception if selected key is out-of-range
	if (!inRange(key, 0, 16))
	{
	    stringstream ss;
	    ss << "Invalid key of " << dec << int(key);
	    throw out_of_range(ss.str().c_str());
	}

	keypad.at(key) = is_pressed;
    }

    bool Chip8Logic::isPressed(int key)
    {
	// Throw exception if selected key is out-of-range
	if (!inRange(key, 0, 16))
	{
	    stringstream ss;
	    ss << "Invalid key of " << dec << int(key);
	    throw out_of_range(ss.str().c_str());
	}

	return keypad.at(key);
    }

    uint16_t Chip8Logic::getInstruction()
    {
	// Fetch high byte of instruction
	uint8_t high = memory.at(pc);
	pc += 1; // Increment PC
	// Fetch low byte of instruction
	uint8_t low = memory.at(pc);
	pc += 1; // Increment PC
	// Instruction is stored most-significant byte first
	return ((high << 8) | low);
    }

    bool Chip8Logic::getPixel(int x, int y)
    {
	// Throw exception if pixel is out-of-range
	if (!inRange(x, 0, 64) || !inRange(y, 0, 32))
	{
	    stringstream ss;
	    ss << "Invalid framebuffer coordinate of ";
	    ss << "(" << dec << x << "," << dec << y << ")";
	    throw out_of_range(ss.str().c_str());
	}

	// Convert (x, y) offset to one-dimensional offset
	// (x + (y * WIDTH))
	uint32_t offs = (x + (y * 64));
	return display.at(offs);
    }

    // Set pixel at (x, y) to val
    void Chip8Logic::setPixel(int x, int y, bool val)
    {
	// Throw exception if pixel is out-of-range
	if (!inRange(x, 0, 64) || !inRange(y, 0, 32))
	{
	    stringstream ss;
	    ss << "Invalid framebuffer coordinate of ";
	    ss << "(" << dec << x << "," << dec << y << ")";
	    throw out_of_range(ss.str().c_str());
	}

	uint32_t offs = (x + (y * 64));
	display.at(offs) = val;
    }

    uint8_t Chip8Logic::getReg(int reg)
    {
	// Throw exception if selected register is out-of-range
	if (!inRange(reg, 0, 16))
	{
	    stringstream ss;
	    ss << "Invalid register of " << dec << int(reg);
	    throw out_of_range(ss.str().c_str());
	}

	return registers.at(reg);
    }

    void Chip8Logic::setReg(int reg, uint8_t data)
    {
	// Throw exception if selected register is out-of-range
	if (!inRange(reg, 0, 16))
	{
	    stringstream ss;
	    ss << "Invalid register of " << dec << int(reg);
	    throw out_of_range(ss.str().c_str());
	}

	registers.at(reg) = data;
    }

    bool Chip8Logic::clockHardware()
    {
	if (delay_timer > 0)
	{
	    delay_timer -= 1;
	}

	if (sound_timer > 0)
	{
	    sound_timer -= 1;
	}

	return (sound_timer > 0);
    }

    // This function displays basic debug output
    void Chip8Logic::debugOutput()
    {
	uint16_t opcode = (memory.at(pc) << 8) | memory.at(pc + 1);
	for (int i = 0; i < 16; i++)
	{
	    cout << "V" << hex << int(i) << ": " << hex << int(getReg(i)) << endl;
	}

	cout << "I: " << hex << int(addr_I) << endl;
	cout << "PC: " << hex << int(pc) << endl;
	cout << "SP: " << hex << int(sp) << endl;
	cout << "Opcode: " << hex << int(opcode) << endl;
	cout << endl;
    }

    // This function executes an individual CHIP8 instruction
    void Chip8Logic::runInstruction()
    {
	// Fetch instruction
	uint16_t instr = getInstruction();

	// The specific opcode is determined by the top 4 bits of the instruction
	int instr_op = (instr >> 12);

	// nnn is the lowest 12 bits of the instruction
	int nnn = (instr & 0xFFF);
	// x is the lower 4 bits of the instruction's high byte
	int x = ((instr >> 8) & 0xF);
	// y is the upper 4 bits of the instruction's low byte
	int y = ((instr >> 4) & 0xF);
	// n is the lowest 4 bits of the instruction
	int n = (instr & 0xF);
	// kk is the lowest 8 bits of the instruction
	int kk = (instr & 0xFF);

	int instr0_op = (instr & 0xF);
	int instrF_op = (instr & 0xFF);

	switch (instr_op)
	{
	    case 0x0:
	    {
		switch (instr0_op)
		{
		    // CLS
		    case 0x0:
		    {
			// Clear the screen
			display.fill(false);
		    }
		    break;
		    // RET
		    case 0xE:
		    {
			// Decrement the stack pointer
			sp = ((sp - 1) % 16);
			// Pop PC off of the stack
			pc = stack.at(sp);
		    }
		    break;
		    default:
		    {
			cout << "Unrecognized opcode of " << hex << int(instr) << endl;
			exit(1);
		    }
		    break;
		}
	    }
	    break;
	    // JP addr
	    case 0x1:
	    {
		// Set program counter to nnn
		pc = nnn;
	    }
	    break;
	    // CALL addr
	    case 0x2:
	    {
		// Push current PC onto stack
		stack.at(sp) = pc;
		// Increment the stack pointer
		sp = ((sp + 1) % 16);
		// Set program counter to nnn
		pc = nnn;
	    }
	    break;
	    // SE Vx, byte
	    case 0x3:
	    {
		// Increment the program counter
		// if Vx is equal to kk
		if (getReg(x) == kk)
		{
		    pc += 2;
		}
	    }
	    break;
	    // SNE Vx, byte
	    case 0x4:
	    {
		// Increment the program counter
		// if Vx is not equal to kk
		if (getReg(x) != kk)
		{
		    pc += 2;
		}
	    }
	    break;
	    // LD VX, byte
	    case 0x6:
	    {
		// Set Vx to KK
		setReg(x, kk);
	    }
	    break;
	    // ADD Vx, byte
	    case 0x7:
	    {
		// Set Vx to (Vx + kk)
		uint8_t regx = getReg(x);
		setReg(x, (regx + kk));
	    }
	    break;
	    // LD I, addr
	    case 0xA:
	    {
		// Set I register to nnn
		addr_I = nnn;
	    }
	    break;
	    // RND Vx, byte
	    case 0xC:
	    {
		// Generate random number between 0 and 255
		int random_byte = (rand() % 256);
		// Set Vx to (random_byte AND kk)
		setReg(x, (random_byte & kk));
	    }
	    break;
	    // DRW Vx, Vy, nibble
	    case 0xD:
	    {
		// Wrap around cooridnates if going beyond
		// the screen boundaries
		int xcoord = (getReg(x) % 64);
		int ycoord = (getReg(y) % 32);

		// Set height
		int height = n;

		// Set VF to 0
		setReg(0xF, 0);

		for (int row = 0; row < height; row++)
		{
		    // Fetch value in memory at (I + row)
		    uint32_t mem_addr = (addr_I + row);
		    uint8_t sprite_byte = memory.at(mem_addr);

		    for (int col = 0; col < 8; col++)
		    {
			bool is_sprite_pixel = testbit(sprite_byte, (7 - col));

			int xpos = (xcoord + col);
			int ypos = (ycoord + row);

			// Fetch pixel at (xpos, ypos)
			bool current_pixel = getPixel(xpos, ypos);

			// Sprite pixel is on
			if (is_sprite_pixel)
			{
			    // Set VF to 1 if current pixel is also on
			    // (aka. sprite collision has occured)
			    if (current_pixel)
			    {
				setReg(0xF, 1);
			    }

			    // Effectively XOR with the sprite pixel
			    setPixel(xpos, ypos, !current_pixel);
			}
		    }
		}
	    }
	    break;
	    case 0xF:
	    {
		switch (instrF_op)
		{
		    // LD Vx, K
		    case 0x0A:
		    {
			bool key_pressed = false;
			int key = 0;

			for (int i = 0; i < 16; i++)
			{
			    if (isPressed(i))
			    {
				key = i;
				key_pressed = true;
				break;
			    }
			}

			if (key_pressed)
			{
			    setReg(x, key);
			}
			else
			{
			    pc -= 2;
			}
		    }
		    break;
		    // ADD I, Vx
		    case 0x1E:
		    {
			// Add together the values of I and Vx, and store the results in I
			addr_I = (addr_I + getReg(x));
		    }
		    break;
		    // LD F, Vx
		    case 0x29:
		    {
			int reg = getReg(x);
			// Set I to the address of the (5-byte) fontset character specified in Vx
			addr_I = (reg * 5);
		    }
		    break;
		    // LD B, Vx
		    case 0x33:
		    {
			int reg = getReg(x);
			// Calculate hundreds, tens and ones places of value
			int hundreds = (reg / 100);
			int tens = ((reg / 10) % 10);
			int ones = (reg % 10);
			// Store values in memory
			memory.at(addr_I) = hundreds;
			memory.at(addr_I + 1) = tens;
			memory.at(addr_I + 2) = ones;
		    }
		    break;
		    // LD [I], Vx
		    case 0x55:
		    {
			for (int i = 0; i <= x; i++)
			{
			    memory.at(addr_I + i) = getReg(i);
			}
		    }
		    break;
		    // LD Vx, [I]
		    case 0x65:
		    {
			for (int i = 0; i <= x; i++)
			{
			    setReg(i, memory.at(addr_I + i));
			}
		    }
		    break;
		    default:
		    {
			cout << "Unrecognized opcode of " << hex << int(instr) << endl;
			exit(1);
		    }
		    break;
		}
	    }
	    break;
	    default:
	    {
		cout << "Unrecognized opcode of " << hex << int(instr) << endl;
		exit(1);
	    }
	    break;
	}
    }

    Chip8Core::Chip8Core()
    {
	core = make_unique<Chip8Logic>();
    }

    Chip8Core::~Chip8Core()
    {

    }

    void Chip8Core::setFrontend(mbCHIP8Frontend *cb)
    {
	front = cb;
    }

    bool Chip8Core::init()
    {
	core->init();

	if (front != NULL)
	{
	    if (!front->init())
	    {
		return false;
	    }
	}

	cout << "mbCHIP8::Initialized" << endl;
	return true;
    }

    void Chip8Core::shutdown()
    {
	core->shutdown();

	if (front != NULL)
	{
	    front->shutdown();
	}

	cout << "mbCHIP8::Shutting down..." << endl;
    }

    void Chip8Core::runCore()
    {
	core->runInstruction();
	bool is_playing_sound = core->clockHardware();

	if (front != NULL)
	{
	    front->drawPixels();
	    front->playAudio(is_playing_sound);
	}
    }

    bool Chip8Core::loadROM(string filename)
    {
	if (front == NULL)
	{
	    return true;
	}

	return loadROM(front->loadfile(filename));
    }

    bool Chip8Core::loadROM(vector<uint8_t> data)
    {
	return core->loadROM(data);
    }

    bool Chip8Core::getPixel(int x, int y)
    {
	return core->getPixel(x, y);
    }

    void Chip8Core::keyChanged(int key, bool is_pressed)
    {
	core->keyChanged(key, is_pressed);
    }

    void Chip8Core::keyPressed(int key)
    {
	keyChanged(key, true);
    }

    void Chip8Core::keyReleased(int key)
    {
	keyChanged(key, false);
    }
};