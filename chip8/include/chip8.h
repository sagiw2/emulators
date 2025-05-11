#pragma once

#include <cstdint>
#include <random>

const unsigned int START_ADDRESS = 0x200;
const unsigned int FONTSET_SIZE = 80;
const unsigned int FONTSET_ADDRESS = 0x50;
const unsigned int VIDEO_HEIGHT = 32;
const unsigned int VIDEO_WIDTH = 64;

class Chip8
{
public:
    uint8_t memory[4096]{};
    uint8_t V[16]{};
    uint16_t I{};
    uint16_t stack[16]{};
    uint16_t pc{};
    uint8_t sp{};
    uint8_t delayTimer{};
    uint8_t soundTimer{};
    uint8_t keypad[16]{};
	uint32_t video[VIDEO_HEIGHT * VIDEO_WIDTH]{};
	uint16_t opcode;
    using InstructionFunc = void(Chip8::*)(uint16_t);
    InstructionFunc table[16];
    std::default_random_engine randGen;
	std::uniform_int_distribution<uint8_t> randByte;
    uint8_t fontset[FONTSET_SIZE] =
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
        0xF0, 0x80, 0xF0, 0x80, 0x80  // F
    };

    Chip8();
    void Cycle();
    void LoadROM(char const* filename);
    void OP_NULL() {}
    void OP_0(uint16_t opcode);
    void OP_00E0();
    void OP_00EE();
    void OP_1(uint16_t opcode) {OP_1nnn();}
    void OP_1nnn();
    void OP_2(uint16_t opcode) {OP_2nnn();}
    void OP_2nnn();
    void OP_3(uint16_t opcode) {OP_3xkk();}
    void OP_3xkk();
    void OP_4(uint16_t opcode) {OP_4xkk();}
    void OP_4xkk();
    void OP_5(uint16_t opcode) {OP_5xy0();}
    void OP_5xy0();
    void OP_6(uint16_t opcode) {OP_6xkk();}
    void OP_6xkk();
    void OP_7(uint16_t opcode) {OP_7xkk();}
    void OP_7xkk();
    void OP_8(uint16_t opcode);
    void OP_8xy0();
    void OP_8xy1();
    void OP_8xy2();
    void OP_8xy3();
    void OP_8xy4();
    void OP_8xy5();
    void OP_8xy6();
    void OP_8xy7();
    void OP_8xyE();
    void OP_9(uint16_t opcode) {OP_9xy0();}
    void OP_9xy0();
    void OP_A(uint16_t opcode) {OP_Annn();}
    void OP_Annn();
    void OP_B(uint16_t opcode) {OP_Bnnn();}
    void OP_Bnnn();
    void OP_C(uint16_t opcode) {OP_Cxkk();}
    void OP_Cxkk();
    void OP_D(uint16_t opcode) {OP_Dxyn();}
    void OP_Dxyn();
    void OP_E(uint16_t opcode);
    void OP_Ex9E();
    void OP_ExA1();
    void OP_F(uint16_t opcode);
    void OP_Fx07();
    void OP_Fx0A();
    void OP_Fx15();
    void OP_Fx18();
    void OP_Fx1E();
    void OP_Fx29();
    void OP_Fx33();
    void OP_Fx55();
    void OP_Fx65();
};
/*
00E0
00EE
1nnn
2nnn
3xkk
4xkk
5xy0
6xkk
7xkk
8xy0
8xy1
8xy2
8xy3
8xy4
8xy5
8xy6
8xy7
8xyE
9xy0
Annn
Bnnn
Cxkk
Dxyn
Ex9E
ExA1
Fx07
Fx0A
Fx15
Fx18
Fx1E
Fx29
Fx33
Fx55
Fx65
*/


