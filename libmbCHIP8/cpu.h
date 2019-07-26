#ifndef CPU_H_INCLUDED
#define CPU_H_INCLUDED
#include <iostream>
#include <string>
using namespace std;

typedef unsigned char BYTE;
typedef unsigned short WORD;

class Chip8
{

public:

    Chip8();
    ~Chip8();

    // Memory
    BYTE m_GameMemory[4096];

    // Registers
    BYTE m_Registers[16];

    // I register
    WORD m_AddressI;

    // Timers
    BYTE m_DelayTimer; // Delay timer
    BYTE m_SoundTimer; // Sound timer

    WORD m_ProgramCounter; // Program counter
    BYTE m_StackPointer; // Stack pointer

    // Stack
    WORD m_Stack[16];

    // Keys
    BYTE m_KeyData[16];

    // Screen
    BYTE m_ScreenData[64 * 32];

    // Fontset
    BYTE m_Fontset[80] =
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
        0xF0, 0x80, 0xF0, 0x80, 0x80 // F
    };

    // Opcode
    WORD m_Opcode;

    // Functions
    void CPUReset();
    void ClearScreen();

    WORD GetNextOpcode();
    void ExecuteNextOpcode(WORD opcode);


    void DecodeOpcode0(WORD opcode);
    void DecodeOpcode8(WORD opcode);
    void DecodeOpcodeE(WORD opcode);
    void DecodeOpcodeF(WORD opcode);

    void Opcode00EE();
    void Opcode1NNN(WORD opcode);
    void Opcode2NNN(WORD opcode);
    void Opcode3XKK(WORD opcode);
    void Opcode4XKK(WORD opcode);
    void Opcode5XY0(WORD opcode);
    void Opcode6XKK(WORD opcode);
    void Opcode7XKK(WORD opcode);
    void Opcode8XY0(WORD opcode);
    void Opcode8XY1(WORD opcode);
    void Opcode8XY2(WORD opcode);
    void Opcode8XY3(WORD opcode);
    void Opcode8XY4(WORD opcode);
    void Opcode8XY5(WORD opcode);
    void Opcode8XY6(WORD opcode);
    void Opcode8XY7(WORD opcode);
    void Opcode8XYE(WORD opcode);
    void Opcode9XY0(WORD opcode);
    void OpcodeANNN(WORD opcode);
    void OpcodeBNNN(WORD opcode);
    void OpcodeCXKK(WORD opcode);
    void OpcodeDXYN(WORD opcode);
    void OpcodeEX9E(WORD opcode);
    void OpcodeEXA1(WORD opcode);
    void OpcodeFX07(WORD opcode);
    void OpcodeFX0A(WORD opcode);
    void OpcodeFX15(WORD opcode);
    void OpcodeFX18(WORD opcode);
    void OpcodeFX1E(WORD opcode);
    void OpcodeFX29(WORD opcode);
    void OpcodeFX33(WORD opcode);
    void OpcodeFX55(WORD opcode);
    void OpcodeFX65(WORD opcode);

    bool SetPixel(int x, int y);


    // Timer functions
    void DecreaseTimers();


    bool LoadROM(const char *filename);
};


#endif // CPU_H_INCLUDED
