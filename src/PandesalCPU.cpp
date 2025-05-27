#include "PandesalCPU.h"
#include "Bus.h"
#include <iostream>

PandesalCPU::PandesalCPU()
{
	using a = PandesalCPU;
	lookup =
	{{
		&a::NOP_IMP, &a::LDA_IMM, &a::STA_ABS, &a::JMP_ABS, &a::ADC_IMM, &a::AND_IMM, &a::ORA_IMM, &a::XOR_IMM, 
		&a::NOT_IMM, &a::LDX_IMM, &a::LDY_IMM, &a::TAX_IMP, &a::TAY_IMP, &a::STX_ABS, &a::STY_ABS, &a::TSX_IMP,
		&a::TXA_IMP, &a::TXS_IMP, &a::TYA_IMP, &a::SBC_IMM, &a::PHA_IMP, &a::PLA_IMP, &a::PHC_IMP, &a::CLC_IMP,
		&a::HLT_IMP, &a::LDA_ABS, &a::ADC_ABS, &a::RTS_IMP, &a::SBC_ABS, &a::CLV_IMP, &a::BCC_ABS, &a::BCS_ABS,
		&a::BEQ_ABS, &a::BMI_ABS, &a::BNE_ABS, &a::BPL_ABS, &a::BVC_ABS, &a::BVS_ABS, &a::INX_IMP, &a::INY_IMP,
		&a::SEC_IMP, &a::DEX_IMP, &a::DEY_IMP, &a::CMP_IMM, &a::ASL_A,   &a::LSR_A,   &a::ROL_A,   &a::ROR_A,
		&a::ASL_ABS, &a::LSR_ABS, &a::ROL_ABS, &a::ROR_ABS, &a::CPX_IMM, &a::CPY_IMM, &a::CPX_ABS, &a::CPY_ABS,
		&a::LDX_ABS, &a::LDY_ABS, &a::LDA_ABX, &a::LDA_ABY, &a::AND_ABX, &a::AND_ABY, &a::STA_ABX, &a::STA_ABY,
		&a::INC_A,   &a::INC_ABS, &a::STA_IND, &a::ADC_ABX, &a::ADC_ABY, &a::ADD_IMM, &a::SUB_IMM, &a::ADD_ABS,
		&a::SUB_ABS, &a::LD0_IMM, &a::LD1_IMM, &a::LD2_IMM, &a::LD3_IMM, &a::LD4_IMM, &a::LD0_ABS, &a::LD1_ABS,
		&a::LD2_ABS, &a::LD3_ABS, &a::LD4_ABS, &a::TA0_IMP, &a::TA1_IMP, &a::TA2_IMP, &a::TA3_IMP, &a::TA4_IMP,
		&a::T0A_IMP, &a::T1A_IMP, &a::T2A_IMP, &a::T3A_IMP, &a::T4A_IMP, &a::TX0_IMP, &a::TX1_IMP, &a::TX2_IMP,
		&a::TX3_IMP, &a::TX4_IMP, &a::T0X_IMP, &a::T1X_IMP, &a::T2X_IMP, &a::T3X_IMP, &a::T4X_IMP, &a::TY0_IMP,
		&a::TY1_IMP, &a::TY2_IMP, &a::TY3_IMP, &a::TY4_IMP, &a::T0Y_IMP, &a::T1Y_IMP, &a::T2Y_IMP, &a::T3Y_IMP,
		&a::T4Y_IMP, &a::LDA_IND
	}};
}

PandesalCPU::~PandesalCPU()
{
}

void PandesalCPU::clock()
{
	if (halt) return;

	// Fetch & Decode Cycle
	switch (step_counter)
	{
	case 0:
		addr_bus = (addr_bus & 0xFF00) | (PC & 0x00FF);
		break;
	case 1:
		addr_bus = (addr_bus & 0x00FF) | (PC & 0xFF00);
		break;
	case 2:
		IR = read();
		increment();
		break;
	}

	// Execute Cycle
	if (step_counter >= 3)
	{
		(this->*lookup[IR])();
	}

	step_counter++;
	if (instruction_clear)
	{
		step_counter = 0;
		instruction_clear = false;
	}
	if (step_counter > 0x0F) step_counter &= 0x0F;
	return;
}

void PandesalCPU::reset()
{
	A = 0x00;
	X = 0x00;
	Y = 0x00;
	GP0 = 0x00;
	GP1 = 0x00;
	GP2 = 0x00;
	GP3 = 0x00;
	GP4 = 0x00;
	IR = 0x00;
	ALU_B = 0x00;
	ALU_out = 0x00;
	PC = 0x0000;
	addr_bus = 0x0000;
	SP = 0x00;
	status = 0x00;
	step_counter = 0x00;
}

void PandesalCPU::increment()
{
	PC++;
	addr_bus++;
}

void PandesalCPU::SetFlag(FLAGS f, bool v)
{
	if (v)
		status |= f;
	else
		status &= ~f;
}

uint8_t PandesalCPU::GetFlag(FLAGS f)
{
	return ((status & f) > 0) ? 1 : 0;
}

uint8_t PandesalCPU::read()
{
	return bus->cpuRead(addr_bus);
}

void PandesalCPU::write(uint8_t data)
{
	bus->cpuWrite(addr_bus, data);
}

void PandesalCPU::calculateADC()
{
	temp = (uint16_t)A + (uint16_t)ALU_B + (uint16_t)(GetFlag(C));
	SetFlag(C, temp > 255);
	SetFlag(Z, (temp & 0x00FF) == 0);
	SetFlag(O, (~((uint16_t)A ^ (uint16_t)ALU_B) & ((uint16_t)A ^ temp)) & 0x0080);
	SetFlag(N, temp & 0x80);
	ALU_out = temp & 0x00FF;
}

void PandesalCPU::calculateADD()
{
	temp = (uint16_t)A + (uint16_t)ALU_B;
	SetFlag(C, temp > 255);
	SetFlag(Z, (temp & 0x00FF) == 0);
	SetFlag(O, (~((uint16_t)A ^ (uint16_t)ALU_B) & ((uint16_t)A ^ temp)) & 0x0080);
	SetFlag(N, temp & 0x80);
	ALU_out = temp & 0x00FF;
}

void PandesalCPU::calculateSBC()
{
	uint16_t value = ((uint16_t)ALU_B) ^ 0x00FF;
	temp = (uint16_t)A + (uint16_t)ALU_B + (uint16_t)(GetFlag(C));
	SetFlag(C, temp > 255);
	SetFlag(Z, (temp & 0x00FF) == 0);
	SetFlag(O, (temp ^ (uint16_t)A) & (temp ^ value) & 0x0080);
	SetFlag(N, temp & 0x80);
	ALU_out = temp & 0x00FF;
}

void PandesalCPU::calculateSUB()
{
	temp = (uint16_t)A - (uint16_t)ALU_B;
	SetFlag(C, A > ALU_B);
	SetFlag(Z, (temp & 0x00FF) == 0);
	SetFlag(N, temp & 0x80);
	ALU_out = temp & 0x00FF;
}

void PandesalCPU::LDA_IMM()
{
	A = read();
	increment();
	instruction_clear = true;
}

void PandesalCPU::LDA_ABS()
{
	switch (step_counter - 3)
	{
	case 0:
		ALU_B = read();
		increment();
		break;
	case 1:
		addr_bus = (addr_bus & 0x00FF) | (uint16_t)(read() << 8);
		break;
	case 2:
		addr_bus = (addr_bus & 0xFF00) | ALU_B;
		break;
	case 3:
		A = read();
		increment();
		instruction_clear = true;
		break;
	}
}

void PandesalCPU::LDY_ABS()
{
	switch (step_counter - 3)
	{
	case 0:
		ALU_B = read();
		increment();
		break;
	case 1:
		addr_bus = (addr_bus & 0x00FF) | (uint16_t)(read() << 8);
		break;
	case 2:
		addr_bus = (addr_bus & 0xFF00) | ALU_B;
		break;
	case 3:
		Y = read();
		increment();
		instruction_clear = true;
		break;
	}
}

void PandesalCPU::LDA_ABX()
{
	switch (step_counter - 3)
	{
	case 0:
		ALU_B = read();
		increment();
		break;
	case 1:
		addr_bus = (addr_bus & 0x00FF) | (uint16_t)(read() << 8);
		break;
	case 2:
		addr_bus = (addr_bus & 0xFF00) | ALU_B;
		break;
	case 3:
		addr_bus += X;
		break;
	case 4:
		A = read();
		increment();
		instruction_clear = true;
		break;
	}
}

void PandesalCPU::LDA_ABY()
{
	switch (step_counter - 3)
	{
	case 0:
		ALU_B = read();
		increment();
		break;
	case 1:
		addr_bus = (addr_bus & 0x00FF) | (uint16_t)(read() << 8);
		break;
	case 2:
		addr_bus = (addr_bus & 0xFF00) | ALU_B;
		break;
	case 3:
		addr_bus += Y;
		break;
	case 4:
		A = read();
		increment();
		instruction_clear = true;
		break;
	}
}

void PandesalCPU::STA_ABX()
{
	switch (step_counter - 3)
	{
	case 0:
		ALU_B = read();
		increment();
		break;
	case 1:
		addr_bus = (addr_bus & 0x00FF) | (uint16_t)(read() << 8);
		break;
	case 2:
		addr_bus = (addr_bus & 0xFF00) | ALU_B;
		break;
	case 3:
		PC++;
		addr_bus += X;
		break;
	case 4:
		write(A);
		break;
	}
}

void PandesalCPU::STA_ABY()
{
	switch (step_counter - 3)
	{
	case 0:
		ALU_B = read();
		increment();
		break;
	case 1:
		addr_bus = (addr_bus & 0x00FF) | (uint16_t)(read() << 8);
		break;
	case 2:
		addr_bus = (addr_bus & 0xFF00) | ALU_B;
		break;
	case 3:
		PC++;
		addr_bus += Y;
		break;
	case 4:
		write(A);
		break;
	}
}

void PandesalCPU::STX_ABS()
{
	switch (step_counter - 3)
	{
	case 0:
		ALU_B = read();
		increment();
		break;
	case 1:
		addr_bus = (addr_bus & 0x00FF) | (uint16_t)(read() << 8);
		break;
	case 2:
		addr_bus = (addr_bus & 0xFF00) | ALU_B;
		break;
	case 3:
		write(X);
		break;
	case 4:
		increment();
		instruction_clear = true;
		break;
	}
}

void PandesalCPU::STY_ABS()
{
	switch (step_counter - 3)
	{
	case 0:
		ALU_B = read();
		increment();
		break;
	case 1:
		addr_bus = (addr_bus & 0x00FF) | (uint16_t)(read() << 8);
		break;
	case 2:
		addr_bus = (addr_bus & 0xFF00) | ALU_B;
		break;
	case 3:
		write(Y);
		break;
	case 4:
		increment();
		instruction_clear = true;
		break;
	}
}

void PandesalCPU::TAX_IMP()
{
	X = A;
	instruction_clear = true;
}

void PandesalCPU::TAY_IMP()
{
	Y = A;
	instruction_clear = true;
}

void PandesalCPU::TSX_IMP()
{
	X = SP;
	instruction_clear = true;
}

void PandesalCPU::TXA_IMP()
{
	A = X;
	instruction_clear = true;
}

void PandesalCPU::TXS_IMP()
{
	SP = X;
	instruction_clear = true;
}

void PandesalCPU::TYA_IMP()
{
	A = Y;
	instruction_clear = true;
}

void PandesalCPU::STA_ABS()
{
	switch (step_counter - 3)
	{
	case 0:
		ALU_B = read();
		increment();
		break;
	case 1:
		addr_bus = (addr_bus & 0x00FF) | (uint16_t)(read() << 8);
		break;
	case 2:
		addr_bus = (addr_bus & 0xFF00) | ALU_B;
		PC++;
		break;
	case 3:
		write(A);
		instruction_clear = true;
		break;
	}
}

void PandesalCPU::STA_IND()
{
	switch (step_counter - 3)
	{
	case 0:
		ALU_B = read();
		increment();
		break;
	case 1:
		addr_bus = (addr_bus & 0x00FF) | (uint16_t)(read() << 8);
		break;
	case 2:
		addr_bus = (addr_bus & 0xFF00) | ALU_B;
		break;
	case 3:
		ALU_B = read();
		increment();
		break;
	case 4:
		addr_bus = (addr_bus & 0x00FF) | (uint16_t)(read() << 8);
		break;
	case 5:
		addr_bus = (addr_bus & 0xFF00) | ALU_B;
		break;
	case 6:
		write(A);
		break;
	case 7:
		instruction_clear = true;
		break;
	}
}

void PandesalCPU::LDA_IND()
{
	switch (step_counter - 3)
	{
	case 0:
		ALU_B = read();
		increment();
		break;
	case 1:
		addr_bus = (addr_bus & 0x00FF) | (uint16_t)(read() << 8);
		break;
	case 2:
		addr_bus = (addr_bus & 0xFF00) | ALU_B;
		break;
	case 3:
		ALU_B = read();
		increment();
		break;
	case 4:
		addr_bus = (addr_bus & 0x00FF) | (uint16_t)(read() << 8);
		break;
	case 5:
		addr_bus = (addr_bus & 0xFF00) | ALU_B;
		break;
	case 6:
		A = read();
		instruction_clear = true;
		break;
	}
}

void PandesalCPU::LDX_IMM()
{
	X = read();
	increment();
	instruction_clear = true;
}

void PandesalCPU::LDY_IMM()
{
	Y = read();
	increment();
	instruction_clear = true;
}

void PandesalCPU::LDX_ABS()
{
	switch (step_counter - 3)
	{
	case 0:
		ALU_B = read();
		increment();
		break;
	case 1:
		addr_bus = (addr_bus & 0x00FF) | (uint16_t)(read() << 8);
		break;
	case 2:
		addr_bus = (addr_bus & 0xFF00) | ALU_B;
		break;
	case 3:
		X = read();
		increment();
		instruction_clear = true;
		break;
	}
}

void PandesalCPU::LD0_IMM()
{
	GP0 = read();
	increment();
	instruction_clear = true;
}

void PandesalCPU::LD1_IMM()
{
	GP1 = read();
	increment();
	instruction_clear = true;
}

void PandesalCPU::LD2_IMM()
{
	GP2 = read();
	increment();
	instruction_clear = true;
}

void PandesalCPU::LD3_IMM()
{
	GP3 = read();
	increment();
	instruction_clear = true;
}

void PandesalCPU::LD4_IMM()
{
	GP4 = read();
	increment();
	instruction_clear = true;
}

void PandesalCPU::LD0_ABS()
{
	switch (step_counter - 3)
	{
	case 0:
		ALU_B = read();
		increment();
		break;
	case 1:
		addr_bus = (addr_bus & 0x00FF) | (uint16_t)(read() << 8);
		break;
	case 2:
		addr_bus = (addr_bus & 0xFF00) | ALU_B;
		break;
	case 3:
		GP0 = read();
		increment();
		instruction_clear = true;
		break;
	}
}

void PandesalCPU::LD1_ABS()
{
	switch (step_counter - 3)
	{
	case 0:
		ALU_B = read();
		increment();
		break;
	case 1:
		addr_bus = (addr_bus & 0x00FF) | (uint16_t)(read() << 8);
		break;
	case 2:
		addr_bus = (addr_bus & 0xFF00) | ALU_B;
		break;
	case 3:
		GP1 = read();
		increment();
		instruction_clear = true;
		break;
	}
}

void PandesalCPU::LD2_ABS()
{
	switch (step_counter - 3)
	{
	case 0:
		ALU_B = read();
		increment();
		break;
	case 1:
		addr_bus = (addr_bus & 0x00FF) | (uint16_t)(read() << 8);
		break;
	case 2:
		addr_bus = (addr_bus & 0xFF00) | ALU_B;
		break;
	case 3:
		GP2 = read();
		increment();
		instruction_clear = true;
		break;
	}
}

void PandesalCPU::LD3_ABS()
{
	switch (step_counter - 3)
	{
	case 0:
		ALU_B = read();
		increment();
		break;
	case 1:
		addr_bus = (addr_bus & 0x00FF) | (uint16_t)(read() << 8);
		break;
	case 2:
		addr_bus = (addr_bus & 0xFF00) | ALU_B;
		break;
	case 3:
		GP3 = read();
		increment();
		instruction_clear = true;
		break;
	}
}

void PandesalCPU::LD4_ABS()
{
	switch (step_counter - 3)
	{
	case 0:
		ALU_B = read();
		increment();
		break;
	case 1:
		addr_bus = (addr_bus & 0x00FF) | (uint16_t)(read() << 8);
		break;
	case 2:
		addr_bus = (addr_bus & 0xFF00) | ALU_B;
		break;
	case 3:
		GP4 = read();
		increment();
		instruction_clear = true;
		break;
	}
}

void PandesalCPU::TA0_IMP()
{
	GP0 = A;
	instruction_clear = true;
}

void PandesalCPU::TA1_IMP()
{
	GP1 = A;
	instruction_clear = true;
}

void PandesalCPU::TA2_IMP()
{
	GP2 = A;
	instruction_clear = true;
}

void PandesalCPU::TA3_IMP()
{
	GP3 = A;
	instruction_clear = true;
}

void PandesalCPU::TA4_IMP()
{
	GP4 = A;
	instruction_clear = true;
}

void PandesalCPU::T0A_IMP()
{
	A = GP0;
	instruction_clear = true;
}

void PandesalCPU::T1A_IMP()
{
	A = GP1;
	instruction_clear = true;
}

void PandesalCPU::T2A_IMP()
{
	A = GP2;
	instruction_clear = true;
}

void PandesalCPU::T3A_IMP()
{
	A = GP3;
	instruction_clear = true;
}

void PandesalCPU::T4A_IMP()
{
	A = GP4;
	instruction_clear = true;
}

void PandesalCPU::TX0_IMP()
{
	GP0 = X;
	instruction_clear = true;
}

void PandesalCPU::TX1_IMP()
{
	GP1 = X;
	instruction_clear = true;
}

void PandesalCPU::TX2_IMP()
{
	GP2 = X;
	instruction_clear = true;
}

void PandesalCPU::TX3_IMP()
{
	GP3 = X;
	instruction_clear = true;
}

void PandesalCPU::TX4_IMP()
{
	GP4 = X;
	instruction_clear = true;
}

void PandesalCPU::T0X_IMP()
{
	X = GP0;
	instruction_clear = true;
}

void PandesalCPU::T1X_IMP()
{
	X = GP1;
	instruction_clear = true;
}

void PandesalCPU::T2X_IMP()
{
	X = GP2;
	instruction_clear = true;
}

void PandesalCPU::T3X_IMP()
{
	X = GP3;
	instruction_clear = true;
}

void PandesalCPU::T4X_IMP()
{
	X = GP4;
	instruction_clear = true;
}

void PandesalCPU::TY0_IMP()
{
	GP0 = Y;
	instruction_clear = true;
}

void PandesalCPU::TY1_IMP()
{
	GP1 = Y;
	instruction_clear = true;
}

void PandesalCPU::TY2_IMP()
{
	GP2 = Y;
	instruction_clear = true;
}

void PandesalCPU::TY3_IMP()
{
	GP3 = Y;
	instruction_clear = true;
}

void PandesalCPU::TY4_IMP()
{
	GP4 = Y;
	instruction_clear = true;
}

void PandesalCPU::T0Y_IMP()
{
	Y = GP0;
	instruction_clear = true;
}

void PandesalCPU::T1Y_IMP()
{
	Y = GP1;
	instruction_clear = true;
}

void PandesalCPU::T2Y_IMP()
{
	Y = GP2;
	instruction_clear = true;
}

void PandesalCPU::T3Y_IMP()
{
	Y = GP3;
	instruction_clear = true;
}

void PandesalCPU::T4Y_IMP()
{
	Y = GP4;
	instruction_clear = true;
}

void PandesalCPU::PHA_IMP()
{
}

void PandesalCPU::PLA_IMP()
{
}

void PandesalCPU::PHC_IMP()
{
	switch (step_counter - 3)
	{
	case 0:
		addr_bus = (addr_bus & 0xFF00) | SP;
		break;
	case 1:
		addr_bus = (addr_bus & 0x00FF) | (1 << 8);
		break;
	case 2:
		write((PC >> 8));
		break;
	case 3:
		A = SP;
		break;
	case 4:
		ALU_B = 1;
		break;
	case 5:
		calculateSUB();
		SP = ALU_out;
		break;
	case 6:
		addr_bus = (addr_bus & 0xFF00) | SP;
		break;
	case 7:
		write(PC & 0x00FF);
		break;
	case 8:
		A = SP;
		break;
	case 9:
		calculateSUB();
		SP = ALU_out;
		instruction_clear = true;
		break;
	}
}

void PandesalCPU::INX_IMP()
{
	switch (step_counter - 3)
	{
	case 0:
		A = X;
		break;
	case 1:
		ALU_B = 1;
		break;
	case 2:
		calculateADD();
		X = ALU_out;
		instruction_clear = true;
		break;
	}
}

void PandesalCPU::INY_IMP()
{
	switch (step_counter - 3)
	{
	case 0:
		A = Y;
		break;
	case 1:
		ALU_B = 1;
		break;
	case 2:
		calculateADD();
		Y = ALU_out;
		instruction_clear = true;
		break;
	}
}

void PandesalCPU::DEX_IMP()
{
	switch (step_counter - 3)
	{
	case 0:
		A = X;
		break;
	case 1:
		ALU_B = 1;
		break;
	case 2:
		calculateSUB();
		X = ALU_out;
		instruction_clear = true;
		break;
	}
}

void PandesalCPU::DEY_IMP()
{
	switch (step_counter - 3)
	{
	case 0:
		A = Y;
		break;
	case 1:
		ALU_B = 1;
		break;
	case 2:
		calculateSUB();
		Y = ALU_out;
		instruction_clear = true;
		break;
	}
}

void PandesalCPU::INC_A()
{
	switch (step_counter - 3)
	{
	case 0:
		ALU_B = 1;
		break;
	case 1:
		calculateADD();
		ALU_out = temp & 0x00FF;
		A = ALU_out;
		instruction_clear = true;
		break;
	}
}

void PandesalCPU::INC_ABS()
{
	switch (step_counter - 3)
	{
	case 0:
		ALU_B = read();
		increment();
		break;
	case 1:
		addr_bus = (addr_bus & 0x00FF) | (uint16_t)(read() << 8);
		break;
	case 2:
		addr_bus = (addr_bus & 0xFF00) | ALU_B;
		break;
	case 3:
		A = read();
		break;
	case 4:
		ALU_B = 1;
		break;
	case 5:
		calculateADD();
		write(ALU_out);
		break;
	case 6:
		increment();
		instruction_clear = true;
		break;
	}
}

void PandesalCPU::ADC_IMM()
{
	switch (step_counter - 3)
	{
	case 0:
		ALU_B = read();
		break;
	case 1:
		calculateADC();
		A = ALU_out;
		increment();
		instruction_clear = true;
		break;
	}
}

void PandesalCPU::ADC_ABS()
{
	switch (step_counter - 3)
	{
	case 0:
		ALU_B = read();
		increment();
		break;
	case 1:
		addr_bus = (addr_bus & 0x00FF) | (uint16_t)(read() << 8);
		break;
	case 2:
		addr_bus = (addr_bus & 0xFF00) | ALU_B;
		break;
	case 3:
		ALU_B = read();
		increment();
		break;
	case 4:
		calculateADC();
		A = ALU_out;
		instruction_clear = true;
		break;
	}
}

void PandesalCPU::ADC_ABX()
{
	switch (step_counter - 3)
	{
	case 0:
		ALU_B = read();
		increment();
		break;
	case 1:
		addr_bus = (addr_bus & 0x00FF) | (uint16_t)(read() << 8);
		break;
	case 2:
		addr_bus = (addr_bus & 0xFF00) | ALU_B;
		break;
	case 3:
		addr_bus += X;
		break;
	case 4:
		ALU_B = read();
		increment();
		break;
	case 5:
		calculateADC();
		A = ALU_out;
		instruction_clear = true;
		break;
	}
}

void PandesalCPU::ADC_ABY()
{
	switch (step_counter - 3)
	{
	case 0:
		ALU_B = read();
		increment();
		break;
	case 1:
		addr_bus = (addr_bus & 0x00FF) | (uint16_t)(read() << 8);
		break;
	case 2:
		addr_bus = (addr_bus & 0xFF00) | ALU_B;
		break;
	case 3:
		addr_bus += Y;
		break;
	case 4:
		ALU_B = read();
		increment();
		break;
	case 5:
		calculateADC();
		A = ALU_out;
		instruction_clear = true;
		break;
	}
}

void PandesalCPU::SBC_IMM()
{
	switch (step_counter - 3)
	{
	case 0:
		ALU_B = read();
		break;
	case 1:
		calculateSBC();
		A = ALU_out;
		increment();
		instruction_clear = true;
	}
}

void PandesalCPU::SBC_ABS()
{
	switch (step_counter - 3)
	{
	case 0:
		ALU_B = read();
		increment();
		break;
	case 1:
		addr_bus = (addr_bus & 0x00FF) | (uint16_t)(read() << 8);
		break;
	case 2:
		addr_bus = (addr_bus & 0xFF00) | ALU_B;
		break;
	case 3:
		ALU_B = read();
		increment();
		break;
	case 4:
		calculateSBC();
		ALU_out = temp & 0x00FF;
		A = ALU_out;
		instruction_clear = true;
		break;
	}
}

void PandesalCPU::ADD_IMM()
{
	switch (step_counter - 3)
	{
	case 0:
		ALU_B = read();
		break;
	case 1:
		calculateADD();
		A = ALU_out;
		increment();
		instruction_clear = true;
	}	
}

void PandesalCPU::SUB_IMM()
{
	switch (step_counter - 3)
	{
	case 0:
		ALU_B = read();
		break;
	case 1:
		calculateSUB();
		A = ALU_out;
		increment();
		instruction_clear = true;
	}
}

void PandesalCPU::ADD_ABS()
{
	switch (step_counter - 3)
	{
	case 0:
		ALU_B = read();
		increment();
		break;
	case 1:
		addr_bus = (addr_bus & 0x00FF) | (uint16_t)(read() << 8);
		break;
	case 2:
		addr_bus = (addr_bus & 0xFF00) | ALU_B;
		break;
	case 3:
		ALU_B = read();
		increment();
		break;
	case 4:
		break;
	case 5:
		calculateADD();
		A = ALU_out;
		instruction_clear = true;
		break;
	}
}

void PandesalCPU::SUB_ABS()
{
	switch (step_counter - 3)
	{
	case 0:
		ALU_B = read();
		increment();
		break;
	case 1:
		addr_bus = (addr_bus & 0x00FF) | (uint16_t)(read() << 8);
		break;
	case 2:
		addr_bus = (addr_bus & 0xFF00) | ALU_B;
		break;
	case 3:
		ALU_B = read();
		increment();
		break;
	case 4:
		calculateSUB();
		A = ALU_out;
		instruction_clear = true;
		break;
	}
}

void PandesalCPU::AND_ABX()
{
	switch (step_counter - 3)
	{
	case 0:
		ALU_B = read();
		increment();
		break;
	case 1:
		addr_bus = (addr_bus & 0x00FF) | (uint16_t)(read() << 8);
		break;
	case 2:
		addr_bus = (addr_bus & 0xFF00) | ALU_B;
		break;
	case 3:
		addr_bus += X;
		break;
	case 4:
		ALU_B = read();
		increment();
		break;
	case 5:
		uint8_t temp = A & ALU_B;
		SetFlag(C, 0);
		SetFlag(Z, temp == 0);
		SetFlag(O, (~((uint16_t)A ^ (uint16_t)ALU_B) & ((uint16_t)A ^ temp)) & 0x0080);
		SetFlag(N, temp & 0x80);
		ALU_out = temp & 0x00FF;
		A = ALU_out;
		instruction_clear = true;
	}
}

void PandesalCPU::AND_ABY()
{
	switch (step_counter - 3)
	{
	case 0:
		ALU_B = read();
		increment();
		break;
	case 1:
		addr_bus = (addr_bus & 0x00FF) | (uint16_t)(read() << 8);
		break;
	case 2:
		addr_bus = (addr_bus & 0xFF00) | ALU_B;
		break;
	case 3:
		addr_bus += Y;
		break;
	case 4:
		ALU_B = read();
		increment();
		break;
	case 5:
		uint8_t temp = A & ALU_B;
		SetFlag(C, 0);
		SetFlag(Z, temp == 0);
		SetFlag(O, (~((uint16_t)A ^ (uint16_t)ALU_B) & ((uint16_t)A ^ temp)) & 0x0080);
		SetFlag(N, temp & 0x80);
		ALU_out = temp & 0x00FF;
		A = ALU_out;
		instruction_clear = true;
	}
}

void PandesalCPU::AND_IMM()
{
	switch (step_counter - 3)
	{
	case 0:
		ALU_B = read();
		break;
	case 1:
		uint8_t temp = A & ALU_B;
		SetFlag(C, 0);
		SetFlag(Z, temp == 0);
		SetFlag(O, (~((uint16_t)A ^ (uint16_t)ALU_B) & ((uint16_t)A ^ temp)) & 0x0080);
		SetFlag(N, temp & 0x80);
		ALU_out = temp & 0x00FF;
		A = ALU_out;
		increment();
		instruction_clear = true;
		break;
	}
}

void PandesalCPU::ORA_IMM()
{
	switch (step_counter - 3)
	{
	case 0:
		ALU_B = read();
		break;
	case 1:
		uint8_t temp = A | ALU_B;
		SetFlag(C, 0);
		SetFlag(Z, temp == 0);
		SetFlag(O, (~((uint16_t)A ^ (uint16_t)ALU_B) & ((uint16_t)A ^ temp)) & 0x0080);
		SetFlag(N, temp & 0x80);
		ALU_out = temp & 0x00FF;
		A = ALU_out;
		increment();
		instruction_clear = true;
		break;
	}
}

void PandesalCPU::XOR_IMM()
{
	switch (step_counter - 3)
	{
	case 0:
		ALU_B = read();
		break;
	case 1:
		uint8_t temp = A ^ ALU_B;
		SetFlag(Z, temp == 0);
		SetFlag(O, (~((uint16_t)A ^ (uint16_t)ALU_B) & ((uint16_t)A ^ temp)) & 0x0080);
		SetFlag(N, temp & 0x80);
		ALU_out = temp & 0x00FF;
		A = ALU_out;
		increment();
		instruction_clear = true;
		break;
	}
}

void PandesalCPU::NOT_IMM()
{
	uint8_t temp = ~A;
	SetFlag(Z, temp == 0);
	SetFlag(O, (~((uint16_t)A ^ (uint16_t)ALU_B) & ((uint16_t)A ^ temp)) & 0x0080);
	SetFlag(N, temp & 0x80);
	ALU_out = temp & 0x00FF;
	A = ALU_out;
	increment();
	instruction_clear = true;
}

void PandesalCPU::ASL_A()
{
	temp = (uint16_t)(A << 1);
	SetFlag(C, temp & 0xFF00);
	SetFlag(Z, (temp & 0x00FF) == 0);
	SetFlag(N, temp & 0x80);
	ALU_out = temp & 0x00FF;
	A = ALU_out;
	instruction_clear = true;
}

void PandesalCPU::LSR_A()
{
	temp = (uint16_t)(A >> 1);
	SetFlag(C, A & 0x0001);
	SetFlag(Z, (temp & 0x00FF) == 0);
	SetFlag(N, temp & 0x80);
	ALU_out = temp & 0x00FF;
	A = ALU_out;
	instruction_clear = true;
}

void PandesalCPU::ROL_A()
{
	temp = (uint16_t)(A << 1) | GetFlag(C);
	SetFlag(C, temp & 0xFF00);
	SetFlag(Z, (temp & 0x00FF) == 0);
	SetFlag(N, temp & 0x80);
	ALU_out = temp & 0x00FF;
	A = ALU_out;
	instruction_clear = true;
}

void PandesalCPU::ROR_A()
{
	temp = (uint16_t)(A >> 1) | (uint16_t)(GetFlag(C) << 7);
	SetFlag(C, A & 0x0001);
	SetFlag(Z, (temp & 0x00FF) == 0);
	SetFlag(N, temp & 0x80);
	ALU_out = temp & 0x00FF;
	A = ALU_out;
	instruction_clear = true;
}

void PandesalCPU::ASL_ABS()
{
	switch (step_counter - 3)
	{
	case 0:
		ALU_B = read();
		increment();
		break;
	case 1:
		addr_bus = (addr_bus & 0x00FF) | (uint16_t)(read() << 8);
		break;
	case 2:
		addr_bus = (addr_bus & 0xFF00) | ALU_B;
		break;
	case 3:
		A = read();
		break;
	case 4:
		temp = (uint16_t)(A << 1);
		SetFlag(C, temp & 0xFF00);
		SetFlag(Z, (temp & 0x00FF) == 0);
		SetFlag(N, temp & 0x80);
		ALU_out = temp & 0x00FF;
		A = ALU_out;
		break;
	case 5:
		write(A);
		break;
	case 6:
		increment();
		instruction_clear = true;
	}
}

void PandesalCPU::LSR_ABS()
{
	switch (step_counter - 3)
	{
	case 0:
		ALU_B = read();
		increment();
		break;
	case 1:
		addr_bus = (addr_bus & 0x00FF) | (uint16_t)(read() << 8);
		break;
	case 2:
		addr_bus = (addr_bus & 0xFF00) | ALU_B;
		break;
	case 3:
		A = read();
		break;
	case 4:
		temp = (uint16_t)(A >> 1);
		SetFlag(C, A & 0x0001);
		SetFlag(Z, (temp & 0x00FF) == 0);
		SetFlag(N, temp & 0x80);
		ALU_out = temp & 0x00FF;
		A = ALU_out;
		break;
	case 5:
		write(A);
		break;
	case 6:
		increment();
		instruction_clear = true;
	}
}

void PandesalCPU::ROL_ABS()
{
	switch (step_counter - 3)
	{
	case 0:
		ALU_B = read();
		increment();
		break;
	case 1:
		addr_bus = (addr_bus & 0x00FF) | (uint16_t)(read() << 8);
		break;
	case 2:
		addr_bus = (addr_bus & 0xFF00) | ALU_B;
		break;
	case 3:
		A = read();
		break;
	case 4:
		temp = (uint16_t)(A << 1) | GetFlag(C);
		SetFlag(C, temp & 0xFF00);
		SetFlag(Z, (temp & 0x00FF) == 0);
		SetFlag(N, temp & 0x80);
		ALU_out = temp & 0x00FF;
		A = ALU_out;
		break;
	case 5:
		write(A);
		break;
	case 6:
		increment();
		instruction_clear = true;
	}
}

void PandesalCPU::ROR_ABS()
{
	switch (step_counter - 3)
	{
	case 0:
		ALU_B = read();
		increment();
		break;
	case 1:
		addr_bus = (addr_bus & 0x00FF) | (uint16_t)(read() << 8);
		break;
	case 2:
		addr_bus = (addr_bus & 0xFF00) | ALU_B;
		break;
	case 3:
		A = read();
		break;
	case 4:
		temp = (uint16_t)(A >> 1) | (uint16_t)(GetFlag(C) << 7);
		SetFlag(C, A & 0x0001);
		SetFlag(Z, (temp & 0x00FF) == 0);
		SetFlag(N, temp & 0x80);
		ALU_out = temp & 0x00FF;
		A = ALU_out;
		break;
	case 5:
		write(A);
		break;
	case 6:
		increment();
		instruction_clear = true;
	}
}

void PandesalCPU::CLC_IMP()
{
	switch (step_counter - 3)
	{
	case 0:
		ALU_B = IR;
		break;
	case 1:
		A = status;
		break;
	case 2:
		ALU_out = A & ALU_B;
		status = ALU_out;
		instruction_clear = true;
		break;
	}
}

void PandesalCPU::SEC_IMP()
{
	switch (step_counter - 3)
	{
	case 0:
		ALU_B = IR;
		break;
	case 1:
		A = status;
		break;
	case 2:
		ALU_out = A | ALU_B;
		status = ALU_out;
		instruction_clear = true;
		break;
	}
}

void PandesalCPU::CLV_IMP()
{
	switch (step_counter - 3)
	{
	case 0:
		ALU_B = IR;
		break;
	case 1:
		A = status;
		break;
	case 2:
		ALU_out = A & ALU_B;
		status = ALU_out;
		instruction_clear = true;
		break;
	}
}

void PandesalCPU::CMP_IMM()
{
	switch (step_counter - 3)
	{
	case 0:
		ALU_B = read();
		break;
	case 1:
		calculateSUB();
		increment();
		instruction_clear = true;
		break;
	}
}

void PandesalCPU::CPX_IMM()
{
	switch (step_counter - 3)
	{
	case 0:
		ALU_B = read();
		break;
	case 1:
		A = X;
		break;
	case 2:
		calculateSUB();
		increment();
		instruction_clear = true;
	}
}

void PandesalCPU::CPY_IMM()
{
	switch (step_counter - 3)
	{
	case 0:
		ALU_B = read();
		break;
	case 1:
		A = Y;
		break;
	case 2:
		calculateSUB();
		increment();
		instruction_clear = true;
	}
}

void PandesalCPU::CPX_ABS()
{
	switch (step_counter - 3)
	{
	case 0:
		ALU_B = read();
		increment();
		break;
	case 1:
		addr_bus = (addr_bus & 0x00FF) | (uint16_t)(read() << 8);
		break;
	case 2:
		addr_bus = (addr_bus & 0xFF00) | ALU_B;
		break;
	case 3:
		ALU_B = read();
		increment();
		break;
	case 4:
		A = X;
		break;
	case 5:
		calculateSUB();
		instruction_clear = true;
	}
}

void PandesalCPU::CPY_ABS()
{
	switch (step_counter - 3)
	{
	case 0:
		ALU_B = read();
		increment();
		break;
	case 1:
		addr_bus = (addr_bus & 0x00FF) | (uint16_t)(read() << 8);
		break;
	case 2:
		addr_bus = (addr_bus & 0xFF00) | ALU_B;
		break;
	case 3:
		ALU_B = read();
		increment();
		break;
	case 4:
		A = Y;
		break;
	case 5:
		calculateSUB();
		instruction_clear = true;
		break;
	}
}

void PandesalCPU::BCC_ABS()
{
	switch (step_counter - 3)
	{
	case 0:
		if (GetFlag(C) != 0)
		{
			increment();
			increment();
			instruction_clear = true;
		}
		break;
	case 1:
		PC = (PC & 0xFF00) | read();
		addr_bus++;
		break;
	case 2:
		PC = (PC & 0x00FF) | (uint16_t)(read() << 8);
		instruction_clear = true;
		break;
	}
}

void PandesalCPU::BCS_ABS()
{
	switch (step_counter - 3)
	{
	case 0:
		if (GetFlag(C) != 1)
		{
			increment();
			increment();
			instruction_clear = true;
		}
		break;
	case 1:
		PC = (PC & 0xFF00) | read();
		addr_bus++;
		break;
	case 2:
		PC = (PC & 0x00FF) | (uint16_t)(read() << 8);
		instruction_clear = true;
		break;
	}
}

void PandesalCPU::BEQ_ABS()
{
	switch (step_counter - 3)
	{
	case 0:
		if (GetFlag(Z) != 1)
		{
			increment();
			increment();
			instruction_clear = true;
		}
		break;
	case 1:
		PC = (PC & 0xFF00) | read();
		addr_bus++;
		break;
	case 2:
		PC = (PC & 0x00FF) | (uint16_t)(read() << 8);
		instruction_clear = true;
		break;
	}
}

void PandesalCPU::BMI_ABS()
{
	switch (step_counter - 3)
	{
	case 0:
		if (GetFlag(N) != 1)
		{
			increment();
			increment();
			instruction_clear = true;
		}
		break;
	case 1:
		PC = (PC & 0xFF00) | read();
		addr_bus++;
		break;
	case 2:
		PC = (PC & 0x00FF) | (uint16_t)(read() << 8);
		instruction_clear = true;
		break;
	}
}

void PandesalCPU::BNE_ABS()
{
	switch (step_counter - 3)
	{
	case 0:
		if (GetFlag(Z) != 0)
		{
			increment();
			increment();
			instruction_clear = true;
		}
		break;
	case 1:
		PC = (PC & 0xFF00) | read();
		addr_bus++;
		break;
	case 2:
		PC = (PC & 0x00FF) | (uint16_t)(read() << 8);
		instruction_clear = true;
		break;
	}
}

void PandesalCPU::BPL_ABS()
{
	switch (step_counter - 3)
	{
	case 0:
		if (GetFlag(N) != 0)
		{
			increment();
			increment();
			instruction_clear = true;
		}
		break;
	case 1:
		PC = (PC & 0xFF00) | read();
		addr_bus++;
		break;
	case 2:
		PC = (PC & 0x00FF) | (uint16_t)(read() << 8);
		instruction_clear = true;
		break;
	}
}

void PandesalCPU::BVC_ABS()
{
	switch (step_counter - 3)
	{
	case 0:
		if (GetFlag(O) != 0)
		{
			increment();
			increment();
			instruction_clear = true;
		}
		break;
	case 1:
		PC = (PC & 0xFF00) | read();
		addr_bus++;
		break;
	case 2:
		PC = (PC & 0x00FF) | (uint16_t)(read() << 8);
		instruction_clear = true;
		break;
	}
}

void PandesalCPU::BVS_ABS()
{
	switch (step_counter - 3)
	{
	case 0:
		if (GetFlag(O) != 1)
		{
			increment();
			increment();
			instruction_clear = true;
		}
		break;
	case 1:
		PC = (PC & 0xFF00) | read();
		addr_bus++;
		break;
	case 2:
		PC = (PC & 0x00FF) | (uint16_t)(read() << 8);
		instruction_clear = true;
		break;
	}
}

void PandesalCPU::JMP_ABS()
{
	switch (step_counter - 3)
	{
	case 0:
		ALU_B = read();
		increment();
		break;
	case 1:
		PC = (PC & 0x00FF) | (uint16_t)(read() << 8);
		break;
	case 2:
		PC = (PC & 0xFF00) | ALU_B;
		instruction_clear = true;
	}
}

void PandesalCPU::RTS_IMP()
{
	switch (step_counter - 3)
	{
	case 0:
		A = SP;
		break;
	case 1:
		ALU_B = 1;
		break;
	case 2:
		calculateADD();
		SP = ALU_out;
		break;
	case 3:
		addr_bus = (addr_bus & 0xFF00) | SP;
		break;
	case 4:
		addr_bus = (addr_bus & 0x00FF) | (1 << 8);
		break;
	case 5:
		PC = (PC & 0xFF00) | read();
		break;
	case 6:
		A = SP;
		break;
	case 7:
		calculateADD();
		SP = ALU_out;
		increment();
		break;
	case 8:
		PC = (PC & 0x00FF) | (uint16_t)(read() << 8);
		break;
	case 9:
		increment();
		increment();
		instruction_clear = true;
		break;
	}
}

void PandesalCPU::NOP_IMP()
{
	instruction_clear = true;
}

void PandesalCPU::HLT_IMP()
{
	halt = true;
	instruction_clear = true;
}
