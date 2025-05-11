#include "chip8.h"
#include "SDLPlatfrom.h"
#include <chrono>
#include <iostream>


int main(int argc, char *argv[])
{
    if (argc != 4)
	{
		std::cerr << "Usage: " << argv[0] << " <Scale> <Delay> <ROM>\n";
		std::exit(EXIT_FAILURE);
	}

    int videoScale = std::stoi(argv[1]);
	int cycleDelay = std::stoi(argv[2]);
	char const* romFilename = argv[3];

    SDL_Platform platform("CHIP-8 Interpeter", VIDEO_WIDTH * videoScale, VIDEO_HEIGHT * videoScale, VIDEO_WIDTH, VIDEO_HEIGHT);

    Chip8 chip;
    chip.LoadROM(romFilename);

    int videoPitch = sizeof(chip.video[0]) * VIDEO_WIDTH;

    auto lastCycleTime = std::chrono::high_resolution_clock::now();
    auto lastTimerUpdate  = std::chrono::high_resolution_clock::now();
	const float timerInterval = 1000.0f / 60.0f;
	bool quit = false;

    while (!quit)
	{
		quit = platform.ProcessInput(chip.keypad);

		auto currentTime = std::chrono::high_resolution_clock::now();
		float dt = std::chrono::duration<float, std::chrono::milliseconds::period>(currentTime - lastCycleTime).count();

		if (dt > cycleDelay)
		{
			lastCycleTime = currentTime;
			chip.Cycle();
			platform.Update(chip.video, videoPitch);
		}

		float timerDt = std::chrono::duration<float, std::chrono::milliseconds::period>(currentTime - lastTimerUpdate).count();
		if (timerDt > timerInterval)
		{
			lastTimerUpdate = currentTime;
			if (chip.delayTimer > 0)
				--chip.delayTimer;
			if (chip.soundTimer > 0)
				--chip.soundTimer;
		}
	}

    return 0;
}

