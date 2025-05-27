#pragma once
#define _CRT_SECURE_NO_WARNINGS

#include "SDL.h"
#include "Bus.h"

#define DEFAULT_WINDOW_WIDTH 800
#define DEFAULT_WINDOW_HEIGHT 500
#define EMULATOR_SCREEN_WIDTH 128
#define EMULATOR_SCREEN_HEIGHT 128

Bus bus;
bool done = false;
SDL_Texture* frame = nullptr;
uint8_t* ptr_frame_buffer = nullptr;

double CPU_freq = 0;
Uint64 last_time = SDL_GetPerformanceCounter();
double perf_freq = double(SDL_GetPerformanceFrequency());

void handleSDLEvents(SDL_Window*& window);
bool initSDL(SDL_Window*& window, SDL_Renderer*& renderer);
void renderFramebuffer(SDL_Renderer*& renderer, SDL_Texture*& frame, const uint8_t* framebuffer);
void debugPrint();