#include "Screen.h"

Screen::Screen()
{
	memset(frame_buffer.get(), 0, SCREEN_WIDTH * SCREEN_HEIGHT * 3);
}

Screen::~Screen()
{
}

void Screen::write(uint16_t addr, uint8_t data)
{
	size_t offset = addr * 3;
	frame_buffer[offset + 0] = xterm256LUT[data].r;
	frame_buffer[offset + 1] = xterm256LUT[data].g;
	frame_buffer[offset + 2] = xterm256LUT[data].b;
}
