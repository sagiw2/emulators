#include <cstdint>
#include <cstring>
#include <fstream>
#include <memory>
#include <chrono>
#include <algorithm>
#include <iterator>
#include <iostream>

#include "chip8.h"


Chip8::Chip8() : randGen(std::chrono::system_clock::now().time_since_epoch().count())
{
    pc = START_ADDRESS;

	std::memcpy(&memory[FONTSET_ADDRESS], fontset, FONTSET_SIZE);

    randByte = std::uniform_int_distribution<uint8_t>(0, 255U);

    // set up opcode table with function pointers
    table[0x0] = &Chip8::OP_0;
    table[0x1] = &Chip8::OP_1;
    table[0x2] = &Chip8::OP_2;
    table[0x3] = &Chip8::OP_3;
    table[0x4] = &Chip8::OP_4;
    table[0x5] = &Chip8::OP_5;
    table[0x6] = &Chip8::OP_6;
    table[0x7] = &Chip8::OP_7;
    table[0x8] = &Chip8::OP_8;
    table[0x9] = &Chip8::OP_9;
    table[0xA] = &Chip8::OP_A;
    table[0xB] = &Chip8::OP_B;
    table[0xC] = &Chip8::OP_C;
    table[0xD] = &Chip8::OP_D;
    table[0xE] = &Chip8::OP_E;
    table[0xF] = &Chip8::OP_F;
}

void Chip8::Cycle()
{
    opcode = (memory[pc] << 8u) | memory[pc + 1];

    pc += 2;

    uint8_t firstNibble = (opcode & 0xF000u) >> 12;
    (this->*table[firstNibble])(opcode);

}

void Chip8::LoadROM(char const *filename)
{
    std::ifstream file(filename, std::ios::binary | std::ios::ate);

    if (!file)
    {
        std::cerr << "Failed to open ROM file\n";
        return;
    }
    std::streampos size = file.tellg();
    std::unique_ptr<char[]> buffer = std::make_unique<char[]>(size);

    file.seekg(0, std::ios::beg);
    file.read(buffer.get(), size);
    file.close();

    for (long i = 0; i < size; ++i)
        memory[START_ADDRESS + i] = buffer[i];
}

void Chip8::OP_0(uint16_t opcode)
{
    uint8_t lastNibble = (opcode & 0x000Fu);
    switch (lastNibble)
    {
    case 0xE:
        OP_00EE();
        break;
    case 0x0:
        OP_00E0();
    default:
        OP_NULL();
        break;
    }
}

void Chip8::OP_00E0()
{
	memset(video, 0, sizeof(video));
}

void Chip8::OP_00EE()
{
    pc = stack[--sp];
}

void Chip8::OP_1nnn()
{
    uint16_t addr = opcode & 0x0FFFu;

    pc = addr;
}

void Chip8::OP_2nnn()
{
    uint16_t addr = opcode & 0x0FFFu;

    stack[sp++] = pc;
    pc = addr;
}

void Chip8::OP_3xkk()
{
    uint8_t x = (opcode & 0x0F00u) >> 8u;
    uint8_t kk = opcode & 0x00FFu;

    if (V[x] == kk)
        pc += 2;
}

void Chip8::OP_4xkk()
{
    uint8_t x = (opcode & 0x0F00u) >> 8u;
    uint8_t kk = opcode & 0x00FFu;

    if (V[x] != kk)
        pc += 2;
}

void Chip8::OP_5xy0()
{
    uint8_t x = (opcode & 0x0F00u) >> 8u;
    uint8_t y = (opcode & 0x00F0u) >> 4u;

    if (V[x] == V[y])
        pc += 2;
}

void Chip8::OP_6xkk()
{
    uint8_t x = (opcode & 0x0F00u) >> 8u;
    uint8_t kk = opcode & 0x00FFu;

    V[x] = kk;
}

void Chip8::OP_7xkk()
{
    uint8_t x = (opcode & 0x0F00u) >> 8u;
    uint8_t kk = opcode & 0x00FFu;

    V[x] += kk;
}

void Chip8::OP_8(uint16_t opcode)
{
    uint8_t lastNibble = (opcode & 0x000Fu);
    switch (lastNibble)
    {
    case 0x0:
        OP_8xy0();
        break;
    case 0x1:
        OP_8xy1();
        break;
    case 0x2:
        OP_8xy2();
    case 0x3:
        OP_8xy3();
        break;
    case 0x4:
        OP_8xy4();
    case 0x5:
        OP_8xy5();
        break;
    case 0x6:
        OP_8xy6();
    case 0x7:
        OP_8xy7();
        break;
    case 0xE:
        OP_8xyE();
    default:
        OP_NULL();
        break;
    }
}

void Chip8::OP_8xy0()
{
    uint8_t x = (opcode & 0x0F00u) >> 8u;
    uint8_t y = (opcode & 0x00F0u) >> 4u;

    V[x] = V[y];
}

void Chip8::OP_8xy1()
{
    uint8_t x = (opcode & 0x0F00u) >> 8u;
    uint8_t y = (opcode & 0x00F0u) >> 4u;

    V[x] |= V[y];
}

void Chip8::OP_8xy2()
{
    uint8_t x = (opcode & 0x0F00u) >> 8u;
    uint8_t y = (opcode & 0x00F0u) >> 4u;

    V[x] &= V[y];
}

void Chip8::OP_8xy3()
{
    uint8_t x = (opcode & 0x0F00u) >> 8u;
    uint8_t y = (opcode & 0x00F0u) >> 4u;

    V[x] ^= V[y];
}

void Chip8::OP_8xy4()
{
    uint8_t x = (opcode & 0x0F00u) >> 8u;
    uint8_t y = (opcode & 0x00F0u) >> 4u;
    uint16_t sum = V[x] + V[y];

    if (sum > 255U)
        V[0xF] = 1;
    else
        V[0xF] = 0;

    V[x] = sum & 0xFFu;
}

void Chip8::OP_8xy5()
{
    uint8_t x = (opcode & 0x0F00u) >> 8u;
    uint8_t y = (opcode & 0x00F0u) >> 4u;

    if (V[x] > V[y])
        V[0xF] = 1;
    else
        V[0xF] = 0;

    V[x] -= V[y];
}

void Chip8::OP_8xy6()
{
    uint8_t x = (opcode & 0x0F00u) >> 8u;

    V[0xF] = V[x] & 0x1u;

    V[x] >>= 1;
}

void Chip8::OP_8xy7()
{
    uint8_t x = (opcode & 0x0F00u) >> 8u;
    uint8_t y = (opcode & 0x00F0u) >> 4u;

    if (V[y] > V[x])
        V[0xF] = 1;
    else
        V[0xF] = 0;

    V[x] = V[y] - V[x];
}

void Chip8::OP_8xyE()
{
    uint8_t x = (opcode & 0x0F00u) >> 8u;

    V[0xF] = (V[x] & 0x80u) >> 7u;
    V[x] <<= 1;
}

void Chip8::OP_9xy0()
{
    uint8_t x = (opcode & 0x0F00u) >> 8u;
    uint8_t y = (opcode & 0x00F0u) >> 4u;

    if (V[x] != V[y])
        pc += 2;
}

void Chip8::OP_Annn()
{
    I = opcode & 0x0FFFu;

}

void Chip8::OP_Bnnn()
{
    pc = V[0] + opcode & 0x0FFFu;
}

void Chip8::OP_Cxkk()
{
    uint8_t x = (opcode & 0x0F00u) >> 8u;
    uint8_t kk = opcode & 0x00FFu;

    V[x] = randByte(randGen) & kk;
}

void Chip8::OP_Dxyn()
{
    uint8_t x = (opcode & 0x0F00u) >> 8u;
	uint8_t y = (opcode & 0x00F0u) >> 4u;
	uint8_t n = opcode & 0x000Fu;


    uint8_t positionX = V[x] % VIDEO_WIDTH;
	uint8_t positionY = V[y] % VIDEO_HEIGHT;

    V[0xF] = 0;
    for (unsigned int row{0}; row < n; ++row)
    {
        uint8_t spriteByte = memory[I + row];

        for (unsigned int col{0}; col < 8; ++col)
        {
            uint8_t spritePixle = spriteByte & (0x80u >> col);
            uint32_t* screenPixle = &video[(positionY + row) * VIDEO_WIDTH + positionX + col];

            if (spritePixle)
            {
                if (*screenPixle == 0xFFFFFFFF)
                    V[0xF] = 1;

                *screenPixle ^= 0xFFFFFFFF;
            }
        }
    }
}

void Chip8::OP_E(uint16_t opcode)
{
    uint8_t lastNibble = (opcode & 0x000Fu);
    switch (lastNibble)
    {
    case 0x1:
        OP_ExA1();
        break;
    case 0xE:
        OP_Ex9E();
    default:
        OP_NULL();
        break;
    }
}

void Chip8::OP_Ex9E()
{
    uint8_t x = (opcode & 0x0F00u) >> 8u;
    uint8_t key = V[x];

    if (keypad[key])
        pc += 2;
}

void Chip8::OP_ExA1()
{
    uint8_t x = (opcode & 0x0F00u) >> 8u;
    uint8_t key = V[x];

    if (!keypad[key])
        pc += 2;
}

void Chip8::OP_F(uint16_t opcode)
{
    uint16_t last2Nibbles = (opcode & 0x00FFu);
    switch (last2Nibbles)
    {
    case 0x07:
        OP_Fx07();
        break;
    case 0x0A:
        OP_Fx0A();
        break;
    case 0x15:
        OP_Fx15();
        break;
    case 0x18:
        OP_Fx18();
        break;
    case 0x1E:
        OP_Fx1E();
        break;
    case 0x29:
        OP_Fx29();
        break;
    case 0x33:
        OP_Fx33();
        break;
    case 0x55:
        OP_Fx55();
        break;
    case 0x65:
        OP_Fx65();
        break;
    default:
        OP_NULL();
        break;
    }
}

void Chip8::OP_Fx07()
{
    uint8_t x = (opcode & 0x0F00u) >> 8u;
    V[x] = delayTimer;
}

void Chip8::OP_Fx0A()
{
    uint8_t x = (opcode & 0x0F00u) >> 8u;

    uint8_t* keyItr = std::find(std::begin(keypad), std::end(keypad), 1);
    if (keyItr != std::end(keypad))
    {
        V[x] = keypad[keyItr - std::begin(keypad)];
    }
    else
        pc -= 2;
}

void Chip8::OP_Fx15()
{
    uint8_t x = (opcode & 0x0F00u) >> 8u;
    delayTimer = V[x];
}

void Chip8::OP_Fx18()
{
    uint8_t x = (opcode & 0x0F00u) >> 8u;
    soundTimer = V[x];
}

void Chip8::OP_Fx1E()
{
    uint8_t x = (opcode & 0x0F00u) >> 8u;

    I += V[x];
}

void Chip8::OP_Fx29()
{
    uint8_t x = (opcode & 0x0F00u) >> 8u;
	uint8_t digit = V[x];

	I = FONTSET_ADDRESS + (5 * digit);
}

void Chip8::OP_Fx33()
{
    uint8_t x = (opcode & 0x0F00u) >> 8u;
    uint8_t value = V[x];

    memory[I + 2] = value % 10;
    value /= 10;
    memory[I + 1] = value % 10;
    value /= 10;
    memory[I] = value % 10;
}

void Chip8::OP_Fx55()
{
    uint8_t x = (opcode & 0x0F00u) >> 8u;

    std::memcpy(&memory[I], V, x + 1);
}

void Chip8::OP_Fx65()
{
    uint8_t x = (opcode & 0x0F00u) >> 8u;

    std::memcpy(V, &memory[I], x + 1);
}
