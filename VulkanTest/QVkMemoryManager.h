#pragma once
#include <vulkan/vulkan.h>
#include <vector>
#include "QVkMemory.h"
namespace QVk {
	const uint32_t DEVICE_MEMORY_SIZE = (4 << 20);
	const uint32_t DEVICE_MEMORY_BLOCK_SIZE = (1 << 10);
	class QVkMemoryManager
	{
	private:
		uint32_t allocatedSize;
		uint32_t maxBlockSize;
		uint32_t minBlockSize;
		uint32_t blockLevel;
		std::vector<uint8_t> bitMasks;
		std::vector<uint32_t> wordOffsetsPerLevel;
		VkPhysicalDevice physicalDevice;
		VkDevice logicalDevice;
		QVkMemory deviceMemory;
	private:
		void setBit(uint32_t level, uint32_t index);
		void resetBit(uint32_t level, uint32_t index);
		bool getBit(uint32_t level, uint32_t index);
	public:
		QVkMemoryManager(VkDevice logicalDevice, VkPhysicalDevice physicalDevice, uint32_t memoryIndex);
		~QVkMemoryManager();
		VkDeviceSize allocateMemory(VkDeviceSize allocateSize);
		void freeMemory(VkDeviceSize memoryOffset);
	};
}