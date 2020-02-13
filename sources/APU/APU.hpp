//
// Created by Melefo on 24/01/2020.
//

#ifndef COMSQUARE_APU_HPP
#define COMSQUARE_APU_HPP

#include <memory>
#include "DSP/DSP.hpp"
#include "../Memory/IMemory.hpp"
#include "../Ram/Ram.hpp"

namespace ComSquare::APU
{
	struct InternalRegisters {

		//! @brief The X index register
		uint8_t x;

		//! @brief The YA register
		union {
			struct {
				//! @brief The Accumulator register
				uint8_t a;
				//! @brief The Y Index register
				uint8_t y;
			};
			uint16_t ya;
		};

		//! @brief The Stack pointer register
		uint8_t sp;

		//! @brief The Program counter register
		union {
			struct {
				uint8_t pcl;
				uint8_t pch;
			};
			uint16_t pc;
		};

		//! @brief Program Status Word register
		union {
			struct {
				//! @brief Negative flag
				bool n : 1;
				//! @brief Overflow flag
				bool v : 1;
				//! @brief Direct page flag
				bool p : 1;
				//! @brief Break flag
				bool b : 1;
				//! @brief Half carry flag
				bool h : 1;
				//! @brief Interrupt enabled flag
				bool i : 1;
				//! @brief Zero flag
				bool z : 1;
				//! @brief Carry flag
				bool c : 1;
			};
			uint8_t psw;
		};
	};

	struct Registers
	{
		//! @brief An undocumented register
		uint8_t unknown;

		//! @brief Control Register register
		uint8_t ctrlreg;

		//! @brief DSP Register Address register
		uint8_t dspregAddr;
		//! @brief DSP Register data register
		uint8_t dspregData;

		//! @brief Port 0 register
		uint8_t port0;
		//! @brief Port 1 register
		uint8_t port1;
		//! @brief Port 2 register
		uint8_t port2;
		//! @brief Port 3 register
		uint8_t port3;

		//! @brief Regular Memory register
		uint8_t regmem1;
		//! @brief Another Regular Memory register
		uint8_t regmem2;

		//! @brief Timer-0 register
		uint8_t timer0;
		//! @brief Timer-1 register
		uint8_t timer1;
		//! @brief Timer-2 register
		uint8_t timer2;

		//! @brief Counter-0 register
		uint8_t counter0;
		//! @brief Counter-1 register
		uint8_t counter1;
		//! @brief Counter-2 register
		uint8_t counter2;

	};

	enum StateMode
	{
		Running,
		Sleeping,
		Stopped
	};

	struct MemoryMap
	{
		//! @brief Zero page memory
		std::shared_ptr<Ram::Ram> Page0;
		//! @brief Stack space memory
		std::shared_ptr<Ram::Ram> Page1;
		//! @brief Any-use memory
		std::shared_ptr<Ram::Ram> Memory;
		//! @brief IPL ROM
		std::shared_ptr<Ram::Ram> IPL;
	};

	class APU : public Memory::IMemory {
	private:
		//! @brief All the registers of the APU CPU
		Registers _registers{};
		//! @brief Internal registers of the CPU (accessible from the bus via addr $4200 to $421F).
		InternalRegisters _internalRegisters{};

		//! @brief Internal APU memory separated according to their utility
		MemoryMap _map;

		//! @brief The DSP component used to produce sound
		std::shared_ptr<DSP::DSP> _dsp;

		//! @brief Read from the APU ram.
		//! @param addr The address to read from. The address 0x0000 should refer to the first byte of the register.
		//! @throw InvalidAddress will be thrown if the address is more than $FFFF (the number of register).
		//! @return Return the data.
		uint8_t _internalRead(uint24_t addr);
		//! @brief Write data to the APU ram.
		//! @param addr The address to write to. The address 0x0000 should refer to the first byte of register.
		//! @param data The new value of the register.
		//! @throw InvalidAddress will be thrown if the address is more than $FFFF (the number of register).
		void _internalWrite(uint24_t addr, uint8_t data);

		//! @brief Current state of APU CPU
		StateMode _state = Running;

		//! @brief Keep the number of excess cycles executed to pad the next update
		unsigned int _paddingCycles = 0;

		//! @brief Get direct page offset
		uint24_t _getDirectAddr();
		//! @brief Get absolute direct page offset
		uint24_t _getAbsoluteAddr();

		//! @brief Execute a single instruction.
		//! @return The number of cycles that the instruction took.
		int executeInstruction();

		//! @brief No Operation instruction, do nothing than delay
		int NOP();
		//! @brief Sleep instruction, halts the processor with SLEEP mode
		int SLEEP();
		//! @brief Stop instruction, halts the processor with STOP mode
		int STOP();

		//! @brief Clear Carry instruction, set Carry flag to 0
		int CLRC();
		//! @brief Set Carry instruction, Set Carry flag to 1
		int SETC();
		//! @brief Complement Carry instruction, invert Carry flag value
		int NOTC();
		//! @brief Clear Overflow instruction, Set Overflow & Half Carry flags to 0
		int CLRV();
		//! @brief Clear Direct Page instruction, Set Direct Page flag to 0
		int CLRP();
		//! @brief Set Direct Page instruction, Set Direct Page flag to 1
		int SETP();
		//! @brief Enable interrupts instruction, Set Zero flag to 1
		int EI();
		//! @brief Disable interrupts instruction, Set Zero flag to 0
		int DI();

		//! @brief Set 1-bit instruction, set a bit in direct page
		int SET1(uint24_t dp, uint8_t bit);
		//! @brief Clear 1-bit instruction, clear a bit in direct page
		int CLR1(uint24_t dp, uint8_t bit);
		//! @brief test set 1-bit instruction, Test and set bits with absolute address
		int TSET1();
	public:
		explicit APU();

		//! @brief Read from the internal APU register.
		//! @param addr The address to read from. The address 0x00 should refer to the first byte of the register.
		//! @throw InvalidAddress will be thrown if the address is more than $0F (the number of register).
		//! @return Return the value of the register.
		uint8_t read(uint24_t addr) override;
		//! @brief Write data to the internal APU register.
		//! @param addr The address to write to. The address 0x00 should refer to the first byte of register.
		//! @param data The new value of the register.
		//! @throw InvalidAddress will be thrown if the address is more than $0F (the number of register).
		void write(uint24_t addr, uint8_t data) override;
		//! @brief This function execute the instructions received until the maximum number of cycles is reached.
		//! @return The number of cycles that elapsed.
		void update(unsigned cycles);

		//! @brief This function is executed when the SNES is powered on or the reset button is pushed.
		void reset();
	};
}

#endif //COMSQUARE_APU_HPP
