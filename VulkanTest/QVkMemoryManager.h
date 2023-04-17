#pragma once
#include <vulkan/vulkan.h>
#include <vector>
#include "QVkMemory.h"
#include "QVkDeviceDependent.h"
namespace QVk {
	const uint32_t DEVICE_MEMORY_SIZE = (4 << 20);
	const uint32_t DEVICE_MEMORY_BLOCK_SIZE = (1 << 10);
	typedef uint32_t MEMORY_BLOCK_INDEX;
	class QVkMemoryManager : public QVkDeviceDependent
	{
	private:
		uint32_t allocatedSize;
		uint32_t maxBlockSize;
		uint32_t minBlockSize;
		uint32_t blockLevel;
		std::vector<uint8_t> bitMasks;
		std::vector<uint32_t> wordOffsetsPerLevel;
		QVkMemory deviceMemory;
	private:
		void setBit(uint32_t level, MEMORY_BLOCK_INDEX index);
		void resetBit(uint32_t level, MEMORY_BLOCK_INDEX index);
		bool getBit(uint32_t level, MEMORY_BLOCK_INDEX index);
		MEMORY_BLOCK_INDEX __allocateBlock(uint32_t level);
		void __freeBlock(MEMORY_BLOCK_INDEX blockOffset);
	protected:
		void destroyVkResource();
		QVkMemoryManager(QVkDevice* pDevice, uint32_t memoryIndex);
		~QVkMemoryManager();
	public:
		VkDeviceSize allocateMemory(VkDeviceSize allocateSize);
		VkDeviceSize allocateAlignedMemory(VkDeviceSize allocateSize, uint32_t align);
		void freeMemory(VkDeviceSize memoryOffset);

		std::string getTypeName();

		friend class QVkDevice;
	};
}