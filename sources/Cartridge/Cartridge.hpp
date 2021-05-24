//
// Created by anonymus-raccoon on 1/27/20.
//

#pragma once

#include <string>
#include "../Memory/AMemory.hpp"
#include "../Models/Int24.hpp"
#include "../Memory/ARectangleMemory.hpp"
#include "InterruptVectors.hpp"
#include "../Ram/Ram.hpp"

namespace ComSquare::Cartridge
{
	#define ADDMAPPINGMODE(x, flag) (x = static_cast<MappingMode>(x | (flag)))
	enum MappingMode {
		LoRom = 1u << 0u,
		HiRom = 1u << 1u,
		SlowRom = 1u << 2u,
		FastRom = 1u << 3u,
		ExRom = 1u << 4u,
	};

	struct Header
	{
		//! @brief The name of the game
		std::string gameName;
		//! @brief The memory mapping of the ROM.
		MappingMode mappingMode{};
		//! @brief The rom type (special information about the rom, still don't know what).
		uint8_t romType = 0;
		//! @brief The size (in bytes) of the ram
		unsigned romSize = 0;
		//! @brief The size of the SRom inside the cartridge.
		unsigned sramSize = 0;
		//! @brief Creator license ID code.
		union {
			uint8_t creatorIDs[2];
			uint16_t creatorID = 0;
		};
		//! @brief The version of the game
		uint8_t version = 0;
		//! @brief Checksum complement
		union {
			uint8_t checksumComplements[2];
			uint16_t checksumComplement = 0;
		};
		//! @brief Checksum
		union {
			uint8_t checksums[2];
			uint16_t checksum = 0;
		};
		//! @brief The interrupt vectors used to halt the CPU in native mode
		InterruptVectors nativeInterrupts{};
		//! @brief The interrupt vectors used to halt the CPU in emulation mode
		InterruptVectors emulationInterrupts{};

		Header() = default;
		Header(const Header &) = default;
		Header &operator=(const Header &) = default;
		~Header() = default;
	};

	//! @brief Contains the rom's memory/instructions.
	class Cartridge : public Ram::Ram {
	private:
		//! @brief Sometime the rom's data has an offset for a SMC header. This value indicate the start of the real rom discarding this header.
		uint16_t _romStart = 0;

		//! @brief Get the size of a rom from it's path.
		//! @param romPath The path of the rom to get info from.
		//! @return The size of the rom.
		static size_t getRomSize(const std::string &romPath);
		//! @brief Set the public variable header by parsing the header in the ROM.
		//! @return True if this cartridge has a SCM header, false otherwise.
		bool _loadHeader();
		//! @brief Get the address of the header.
		//! @return The address of this cartridge header.
		uint32_t _getHeaderAddress();
		//! @brief Parse the memory to get a readable header.
		//! @param headerAddress The address you want to parse.
		//! @return A header struct representing the data at the memory address you passed.
		Header _mapHeader(uint32_t headerAddress);
	public:
		//! @brief Load a rom from it's path.
		explicit Cartridge(const std::string &romPath);
		//! @brief The cartridge can't be copied.
		Cartridge(const Cartridge &) = delete;
		//! @brief The cartridge can't be assigned.
		Cartridge &operator=(const Cartridge &) = delete;
		//! @brief Destructor that free the cartridge data.
		~Cartridge() override = default;

		//! @brief The header of the cartridge.
		Header header;
		//! @brief Read from the rom.
		//! @param addr The address to read from. The address 0x0 should refer to the first byte of the rom's memory.
		//! @throw InvalidAddress will be thrown if the address is more than the size of the rom's memory.
		//! @return Return the data at the address.
		uint8_t read(uint24_t addr) override;
		//! @brief Write data to the rom.
		//! @param addr The address to write to. The address 0x0 should refer to the first byte of the rom's memory.
		//! @param data The data to write.
		//! @throw InvalidAddress will be thrown if the address is more than the size of the rom's memory.
		void write(uint24_t addr, uint8_t data) override;
	};
}