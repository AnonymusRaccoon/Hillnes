//
// Created by anonymus-raccoon on 1/27/20.
//

#include <sys/stat.h>
#include <iostream>
#include <cstring>
#include "Cartridge.hpp"
#include "../Exceptions/NotImplementedException.hpp"

namespace ComSquare::Cartridge
{
	size_t Cartridge::getRomSize(const std::string &romPath)
	{
		struct stat info;

		if (stat(romPath.c_str(), &info) < 0)
			throw InvalidRomException("File not found.");
		return info.st_size;
	}

	Cartridge::Cartridge(const std::string &romPath)
	{
		try {
			size_t size = this->getRomSize(romPath);
			FILE *rom = fopen(romPath.c_str(), "rb");

			if (!rom)
				throw InvalidRomException("Could not open the rom file at " + romPath + ". " + strerror(errno));
			this->_size = size;
			this->_data = new unsigned char[size];
			std::memset(this->_data, 0, size);
			fread(this->_data, 1, size, rom);
		} catch (InvalidRomException &ex) {
			std::cerr << "Invalid Rom Error: " << ex.what() << std::endl;
		}
	}

	uint8_t Cartridge::read(uint32_t addr)
	{
		(void)addr;
		throw NotImplementedException();
	}

	void Cartridge::write(uint32_t addr, uint8_t data)
	{
		(void)addr;
		(void)data;
		throw NotImplementedException();
	}
}