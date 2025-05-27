#pragma once
#include <stdint.h>
#include <array>

class Bus;

class PandesalCPU
{
public:
	PandesalCPU();
	~PandesalCPU();

public:
	enum FLAGS
	{
		C = (1 << 3),
		Z = (1 << 2),
		O = (1 << 1),
		N = (1 << 0)
	};

	void clock();
	void reset();
	void increment();
	void ConnectBus(Bus* n) { bus = n; }

	void SetFlag(FLAGS f, bool v);
	uint8_t GetFlag(FLAGS f);

	// Registers
	uint8_t A = 0x00;
	uint8_t X = 0x00;
	uint8_t Y = 0x00;
	uint8_t GP0 = 0x00;
	uint8_t GP1 = 0x00;
	uint8_t GP2 = 0x00;
	uint8_t GP3 = 0x00;
	uint8_t GP4 = 0x00;
	uint8_t IR = 0x00;
	uint8_t step_counter = 0;
	uint8_t ALU_B = 0x00;
	uint8_t ALU_out = 0x00;
	uint16_t PC = 0x0000;
	uint16_t addr_bus = 0x0000;
	uint8_t SP = 0xFF;
	uint8_t status = 0x00; // 4 bit register

	uint16_t temp = 0x0000;
	uint8_t instruction_clear = false;

private:
	Bus* bus = nullptr;
	uint8_t halt = false;

	// Transfer
	void LDA_IMM(); void LDA_ABS();
	void LDY_ABS(); void LDA_ABX();
	void LDA_ABY(); void STA_ABX();
	void STA_ABY(); void STX_ABS();
	void STY_ABS(); void TAX_IMP();
	void TAY_IMP(); void TSX_IMP();
	void TXA_IMP(); void TXS_IMP();
	void TYA_IMP(); void STA_ABS();
	void STA_IND(); void LDA_IND();
	void LDX_IMM(); void LDY_IMM();
	void LDX_ABS();

	void LD0_IMM(); void LD1_IMM();
	void LD2_IMM(); void LD3_IMM();
	void LD4_IMM();

	void LD0_ABS(); void LD1_ABS();
	void LD2_ABS(); void LD3_ABS();
	void LD4_ABS();

	void TA0_IMP(); void TA1_IMP();
	void TA2_IMP(); void TA3_IMP();
	void TA4_IMP();

	void T0A_IMP(); void T1A_IMP();
	void T2A_IMP(); void T3A_IMP();
	void T4A_IMP();

	void TX0_IMP(); void TX1_IMP();
	void TX2_IMP(); void TX3_IMP();
	void TX4_IMP();

	void T0X_IMP(); void T1X_IMP();
	void T2X_IMP(); void T3X_IMP();
	void T4X_IMP();

	void TY0_IMP(); void TY1_IMP();
	void TY2_IMP(); void TY3_IMP();
	void TY4_IMP();

	void T0Y_IMP(); void T1Y_IMP();
	void T2Y_IMP(); void T3Y_IMP();
	void T4Y_IMP();

	// Stack Instructions
	void PHA_IMP();
	void PLA_IMP();
	void PHC_IMP();

	// Increments & Decrements
	void INX_IMP(); void INY_IMP();
	void DEX_IMP(); void DEY_IMP();
	void INC_A();   void INC_ABS();

	// Arithmetic operations
	void ADC_IMM(); void ADC_ABS();
	void ADC_ABX(); void ADC_ABY();
	void SBC_IMM(); void SBC_ABS();
	void ADD_IMM(); void SUB_IMM();
	void ADD_ABS(); void SUB_ABS();

	// Logical operations
	void AND_ABX();
	void AND_ABY();
	void AND_IMM();
	void ORA_IMM();
	void XOR_IMM();
	void NOT_IMM();

	// Shift/Rotate
	void ASL_A(); void LSR_A();
	void ROL_A(); void ROR_A();
	void ASL_ABS(); void LSR_ABS();
	void ROL_ABS(); void ROR_ABS();

	// Flag
	void CLC_IMP(); void SEC_IMP();
	void CLV_IMP();

	// Comparisons
	void CMP_IMM(); void CPX_IMM();
	void CPY_IMM(); void CPX_ABS();
	void CPY_ABS();

	// Conditional Branch
	void BCC_ABS(); void BCS_ABS();
	void BEQ_ABS(); void BMI_ABS();
	void BNE_ABS(); void BPL_ABS();
	void BVC_ABS(); void BVS_ABS();

	// Jumps & Subroutines
	void JMP_ABS();
	void RTS_IMP();

	// Others
	void NOP_IMP();
	void HLT_IMP();

	using instruction = void(PandesalCPU::*)(void);
	std::array<instruction, 256> lookup;

	uint8_t read();
	void write(uint8_t data);

	void calculateADC();
	void calculateADD();
	void calculateSBC();
	void calculateSUB();
};