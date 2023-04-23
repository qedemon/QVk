#pragma once
#include <vulkan/vulkan.h>
#include <vector>
#include "QVkMemory.h"
#include "QVkDeviceDependent.h"
namespace QVk {
	const uint32_t DEVICE_MEMORY_SIZE = (4 << 20);
	const uint32_t DEVICE_MEMORY_BLOCK_SIZE = (1 << 10);
	typedef uint32_t MEMORY_BLOCK_INDEX;
	typedef uint32_t MEMORY_BLOCK_LEVEL;
	class QVkMemoryManager : public QVkDeviceDependent
	{
	private:
		VkDeviceSize allocatedFullSize;
		VkDeviceSize usedSize;
		VkDeviceSize maxBlockSize;
		VkDeviceSize minBlockSize;
		MEMORY_BLOCK_LEVEL blockLevel;
		std::vector<uint8_t> bitMasks;
		std::vector<MEMORY_BLOCK_INDEX> wordOffsetsPerLevel;
		std::vector<MEMORY_BLOCK_LEVEL> memoryBlocksLevels;
		uint32_t memoryIndex;
		QVkMemory deviceMemory;
	private:
		void setBit(MEMORY_BLOCK_LEVEL level, MEMORY_BLOCK_INDEX index);
		void resetBit(MEMORY_BLOCK_LEVEL level, MEMORY_BLOCK_INDEX index);
		bool getBit(MEMORY_BLOCK_LEVEL level, MEMORY_BLOCK_INDEX index);
		MEMORY_BLOCK_INDEX __allocateBlock(MEMORY_BLOCK_LEVEL level);
		void __freeBlock(MEMORY_BLOCK_INDEX blockOffset);
	protected:
		void destroyVkResource();
		QVkMemoryManager(QVkDevice* pDevice, uint32_t memoryIndex);
		~QVkMemoryManager();
		VkDeviceMemory getVkDeviceMemory();
	public:
		uint32_t getMemoryType();
		VkDeviceSize allocateMemory(VkDeviceSize allocateSize);
		VkDeviceSize allocateAlignedMemory(VkDeviceSize allocateSize, VkDeviceSize align);
		void freeMemory(VkDeviceSize memoryOffset);
		void* mapMemory(VkDeviceSize offset, VkDeviceSize size);
		void unmapMemory();

		std::string getTypeName();

		friend class QVkDevice;
		friend class QVkBackedByMemory;
	};
}