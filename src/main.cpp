#include "main.h"
#include <iostream>
#include <SDL.h>
#include <bitset>
#undef main

int main(int argc, char* argv[])
{
	if (argc == 1)
	{
		fprintf(stderr, "Error: No arguments provided.\n");
		fprintf(stderr, "Usage: %s -freq <frequency Hz> -ram <RAM file> -rom <ROM file>\n", argv[0]);
		return 1;
	}

	char* ram_filename = NULL;
	char* rom_filename = NULL;
	for (int i = 1; i < argc; i++) 
	{
		if (strcmp(argv[i], "-ram") == 0 && i + 1 < argc) 
		{
			ram_filename = argv[++i];
		}
		else if (strcmp(argv[i], "-rom") == 0 && i + 1 < argc) 
		{
			rom_filename = argv[++i];
		}
		else if (strcmp(argv[i], "-freq") == 0 && i + 1 < argc) 
		{
			if (sscanf(argv[++i], "%lf", &CPU_freq) != 1)
			{
				fprintf(stderr, "Error: Invalid frequency format.\n");
				return 1;
			}
		}
	}
	if (!ram_filename || CPU_freq == 0)
	{
		fprintf(stderr, "Error: RAM file and/or CPU frequency not specified.\n");
		return 1;
	}
	if (bus.loadRAM(ram_filename) != 0)
	{
		fprintf(stderr, "Error: Invalid RAM file.\n");
		return 1;
	}
	if (rom_filename && bus.loadROM(rom_filename) != 0)
	{
		fprintf(stderr, "Error: Invalid ROM file.\n");
		return 1;
	}

	// Initialize SDL
	SDL_Window* window = nullptr;
	SDL_Renderer* renderer = nullptr;
	if (!initSDL(window, renderer)) return 1;
	
	double cycle_accumulator = 0.0;

	ptr_frame_buffer = bus.screen.frame_buffer.get();
	while (!done)
	{
		handleSDLEvents(window);

		Uint64 now = SDL_GetPerformanceCounter();
		double delta_sec = double(now - last_time) / perf_freq;
		last_time = now;

		cycle_accumulator += delta_sec * CPU_freq;
		int cycles_to_run = int(cycle_accumulator);
		cycle_accumulator -= cycles_to_run;
		for (int i = 0; i < cycles_to_run; i++)
		{
			bus.clock();
		}

		renderFramebuffer(renderer, frame, ptr_frame_buffer);
		SDL_RenderPresent(renderer);
		SDL_Delay(1);
	}

	SDL_DestroyWindow(window);
	SDL_DestroyRenderer(renderer);
	SDL_Quit();
	return 0;
}

void handleSDLEvents(SDL_Window*& window)
{
	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE && event.window.windowID == SDL_GetWindowID(window))
			done = true;

		switch (event.type)
		{
		case SDL_QUIT:
			done = true;
			break;
		}
	}
}

bool initSDL(SDL_Window*& window, SDL_Renderer*& renderer)
{
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0)
	{
		SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
		return false;
	}

	SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
	window = SDL_CreateWindow("Pandesal Emulator", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT, window_flags);
	if (!window)
	{
		SDL_Log("Unable to create SDL window: %s", SDL_GetError());
		return false;
	}

	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	if (!renderer)
	{
		SDL_Log("Unable to create SDL renderer: %s", SDL_GetError());
		return false;
	}

	frame = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB24, SDL_TEXTUREACCESS_STREAMING, SCREEN_WIDTH, SCREEN_HEIGHT);
	if (!frame) 
	{
		SDL_Log("SDL_CreateTexture failed: %s", SDL_GetError());
		return false;
	}
	SDL_RenderSetLogicalSize(renderer, SCREEN_WIDTH, SCREEN_HEIGHT);
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderClear(renderer);
	return true;
}

void renderFramebuffer(SDL_Renderer*& renderer, SDL_Texture*& frame, const uint8_t* framebuffer)
{	
	void *pixels;
	int pitch;
	if (SDL_LockTexture(frame, nullptr, &pixels, &pitch) < 0) 
	{
		SDL_Log("SDL_LockTexture failed: %s", SDL_GetError());
		return;
	}
	memcpy(pixels, framebuffer, EMULATOR_SCREEN_WIDTH * EMULATOR_SCREEN_HEIGHT * 3);
	SDL_UnlockTexture(frame);
	SDL_RenderCopy(renderer, frame, nullptr, nullptr);
}

void debugPrint()
{
	std::cout << "PC: " << +bus.cpu.PC << "\n";
		std::cout << "address bus: " << +bus.cpu.addr_bus<< "\n";
		std::cout << "IR: " << +bus.cpu.IR << "\n";
		std::cout << "Step counter: " << +bus.cpu.step_counter << "\n";
		std::cout << "ALU_out: " << +bus.cpu.ALU_out << "\n";
		std::cout << "A: " << +bus.cpu.A << "\n";
		std::cout << "ALU_B: " << +bus.cpu.ALU_B << "\n";
		std::cout << "status: " << std::bitset<8>(bus.cpu.status) << std::endl;
		std::cout << "SP: " << +bus.cpu.SP << "\n";
		std::cout << "X: " << +bus.cpu.X << "\n";
		std::cout << "Y: " << +bus.cpu.Y << "\n\n";
}
