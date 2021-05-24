//
// Created by anonymus-raccoon on 1/28/20.
//

#pragma once

#include <memory>
#include "AMemory.hpp"

namespace ComSquare::Memory
{
	class MemoryShadow : public AMemory {
	private:
		//! @brief Memory to shadow from.
		std::shared_ptr<IMemory> _initial;
	public:
		//! @brief Create a shadow for the memory given as parameter.
		MemoryShadow(std::shared_ptr<IMemory> initial, uint24_t start, uint24_t end);
		MemoryShadow(const MemoryShadow &) = default;
		MemoryShadow &operator=(const MemoryShadow &) = default;
		~MemoryShadow() = default;

		//! @brief Read from the initial AMemory given.
		//! @param addr The address to read from. The address 0x0 should refer to the first byte of the initial AMemory.
		//! @throw InvalidAddress will be thrown if the address is more than the size of the initial AMemory.
		//! @return Return the data at the address.
		uint8_t read(uint24_t addr) override;
		//! @brief Write data to the ram.
		//! @param addr The address to write to. The address 0x0 should refer to the first byte of the initial AMemory.
		//! @param data The data to write.
		//! @throw InvalidAddress will be thrown if the address is more than the size of the initial AMemory.
		void write(uint24_t addr, uint8_t data) override;
		//! @brief Get the size of the data. This size can be lower than the mapped data.
		//! @return The number of bytes inside this memory.
		virtual uint24_t getSize() const override;
		//! @brief Check if this memory is a mirror or not.
		//! @return True if this memory is a mirror. False otherwise.
		bool isMirror() const override;
		//! @brief Get the name of this accessor (used for debug purpose)
		std::string getName() const override;
		//! @brief Get the component of this accessor (used for debug purpose)
		Component getComponent() const override;
		//! @brief Return the memory accessor this accessor mirror if any
		//! @return nullptr if isMirror is false, the source otherwise.
		std::shared_ptr<IMemory> getMirrored() const override;
	};
}