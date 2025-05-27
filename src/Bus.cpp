#include "Bus.h"

Bus::Bus()
{
	memset(RAM, 0, sizeof(RAM));
	cpu.ConnectBus(this);
}

Bus::~Bus()
{
}

uint8_t Bus::cpuRead(uint16_t addr)
{
	uint8_t data = 0;
	if (addr >= 0x0000 && addr <= 0x7FFF) // RAM
	{
		data = RAM[addr];
	}
	else if (addr >= 0x8000 && addr <= 0xBFFF) // Screen
	{
	}
	else if (addr >= 0xC000 && addr <= 0xFFFF)
	{
		data = ROM[addr];
	}
	return data;
}

void Bus::cpuWrite(uint16_t addr, uint8_t data)
{
	if (addr >= 0x0000 && addr <= 0x7FFF)
	{
		RAM[addr] = data;
	}
	else if (addr >= 0x8000 && addr <= 0xBFFF)
	{
		screen.write((addr % 0x4000), data);	
	}
	else if (addr >= 0xC000 && addr <= 0xFFFF)
	{
	}
}


uint8_t Bus::loadRAM(const std::string file_name)
{
	std::ifstream ifs;
	ifs.open(file_name, std::ifstream::binary);
	if (!ifs.is_open())
	{
		ifs.close();
		return 1;
	}
	ifs.read((char*)RAM, sizeof(RAM));
	ifs.close();
	return 0;
}

uint8_t Bus::loadROM(const std::string file_name)
{
	std::ifstream ifs;
	ifs.open(file_name, std::ios::binary | std::ios::ate);
	if (!ifs.is_open()) return 1;

	std::streamsize size = ifs.tellg();
	if (size <= 0 || size > MAX_ROM_SIZE) return 1;

	if (size < MIN_ROM_SIZE) ROM.resize(MIN_ROM_SIZE);
	else ROM.resize(static_cast<size_t>(size));

	ifs.seekg(0, std::ios::beg);
	if (!ifs.read(reinterpret_cast<char*>(ROM.data()), size)) return 1;
	return 0;
}

void Bus::clock()
{
	cpu.clock();
}

void Bus::reset()
{
	cpu.reset();
	memset(RAM, 0, sizeof(RAM));
}