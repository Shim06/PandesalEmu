#pragma once
#include <string>
#include <fstream>
#include <vector>

#include "PandesalCPU.h"
#include "screen.h"

constexpr size_t BANK_SIZE = 32 * 1024;        // 32 KiB
constexpr size_t MAX_BANKS = 256;
constexpr size_t MAX_ROM_SIZE = BANK_SIZE * MAX_BANKS;
constexpr size_t MIN_ROM_SIZE = 32 * 1024;

class Bus
{
public:
	Bus();
	~Bus();
	std::vector<uint8_t> ROM;

public:
	PandesalCPU cpu;
	Screen screen;

	uint8_t loadRAM(const std::string file_name);
	uint8_t loadROM(const std::string file_name);
	void clock();
	void reset();
	uint8_t cpuRead(uint16_t addr);
	void cpuWrite(uint16_t addr, uint8_t data);
    uint8_t RAM[16384];
};

