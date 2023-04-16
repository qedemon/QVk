#include "QVkMemoryManager.h"

using namespace QVk;

QVkMemoryManager::QVkMemoryManager(VkDevice logicalDevice, VkPhysicalDevice physicalDevice, uint32_t memoryTypeIndex) : 
	physicalDevice(physicalDevice), logicalDevice(logicalDevice), deviceMemory(logicalDevice, physicalDevice)
{
	if (deviceMemory.allocateMemory(memoryTypeIndex, DEVICE_MEMORY_SIZE) != VK_SUCCESS) {
		throw std::exception("QVkMemory allocation error.");
	}

	this->allocatedSize = DEVICE_MEMORY_SIZE;
	this->minBlockSize = DEVICE_MEMORY_BLOCK_SIZE;
	MEMORY_BLOCK_INDEX minBlockCount = allocatedSize / minBlockSize-((allocatedSize%minBlockSize==0)?0:1);
	for (blockLevel = 0; (minBlockCount >> blockLevel) != 0; blockLevel++);
	this->maxBlockSize = minBlockSize << (blockLevel - 1);
	uint32_t totalWordCount = 0;
	MEMORY_BLOCK_INDEX blockCount = minBlockCount;
	for (uint32_t level = 0; level < blockLevel; level++) {
		wordOffsetsPerLevel.push_back(totalWordCount);
		totalWordCount += blockCount / sizeof(decltype(bitMasks.front())) / 8 + ((blockCount % (sizeof(decltype(bitMasks.front())) * 8) > 0) ? 1 : 0);
		blockCount >>= 1;
	}
	wordOffsetsPerLevel.push_back(totalWordCount);

	bitMasks.resize(totalWordCount);
	for (auto bitMask : bitMasks) {
		bitMask = 0;
	}

	MEMORY_BLOCK_INDEX sizePerLevel = minBlockCount;
	for (uint32_t level = 0; level < blockLevel; level++) {
		if (sizePerLevel % 2 == 1) {
			setBit(level, sizePerLevel - 1);
		}
		sizePerLevel >>= 1;
	}
}

QVkMemoryManager::~QVkMemoryManager() {
}

VkDeviceSize QVkMemoryManager::allocateMemory(VkDeviceSize allocateSize) {
	if (allocateSize > maxBlockSize) {
		throw std::exception("QVkMemoryManager::allocateMemory error : allocateSize > maxBlock Size");
	}
	uint32_t level;
	for (level = 0; allocateSize > (minBlockSize << (size_t)level); level++);
	uint32_t blockIndex;
	try {
		blockIndex = __allocateBlock(level);
	}
	catch (std::exception e) {
		throw e;
	}
	return blockIndex*minBlockSize;
}

VkDeviceSize QVkMemoryManager::allocateAlignedMemory(VkDeviceSize allocateSize, uint32_t align) {
	if (allocateSize > maxBlockSize) {
		throw std::exception("QVkMemoryManager::allocateAlignedMemory error : allocateSize > maxBlock Size");
	}
	uint32_t level;
	level = 0;
	while (level < blockLevel) {
		uint32_t blockSize = minBlockSize << level;
		if ((blockSize >= allocateSize) && (blockSize >= align))
			break;
		level++;
	}
	if (level == blockLevel) {
		throw std::exception("QVkMemoryManager::allocateAlignedMemory error : cannot find block level");
	}

	uint32_t blockIndex;
	try {
		blockIndex = __allocateBlock(level);
	}
	catch (std::exception e) {
		throw e;
	}
	VkDeviceSize memoryOffset = blockIndex * (VkDeviceSize)minBlockSize;
	VkDeviceSize remain = memoryOffset % align;
	if (remain != 0)
		memoryOffset += (align - memoryOffset % align);
	return memoryOffset;
}

void QVkMemoryManager::freeMemory(VkDeviceSize memoryOffset) {
	MEMORY_BLOCK_INDEX blockOffset = (MEMORY_BLOCK_INDEX)(memoryOffset / minBlockSize);
	__freeBlock(blockOffset);
}

void QVkMemoryManager::setBit(uint32_t level, MEMORY_BLOCK_INDEX index) {
	const uint32_t WORD_SIZE = sizeof(decltype(bitMasks.front())) * 8;
	uint32_t wordOffset = index / WORD_SIZE;
	uint32_t bitOffset = index % WORD_SIZE;
	bitMasks[wordOffsetsPerLevel[level] + wordOffset] |= (1 << bitOffset);
}

void QVkMemoryManager::resetBit(uint32_t level, MEMORY_BLOCK_INDEX index) {
	const uint32_t WORD_SIZE = sizeof(decltype(bitMasks.front())) * 8;
	uint32_t wordOffset = index / WORD_SIZE;
	uint32_t bitOffset = index % WORD_SIZE;
	bitMasks[wordOffsetsPerLevel[level] + wordOffset] &= ~(1 << bitOffset);
}

bool QVkMemoryManager::getBit(uint32_t level, MEMORY_BLOCK_INDEX index) {
	const uint32_t WORD_SIZE = sizeof(decltype(bitMasks.front())) * 8;
	uint32_t wordOffset = index / WORD_SIZE;
	uint32_t bitOffset = index % WORD_SIZE;
	return (bitMasks[wordOffsetsPerLevel[level] + wordOffset] >> bitOffset) & 1;
}

MEMORY_BLOCK_INDEX QVkMemoryManager::__allocateBlock(uint32_t level) {

	uint32_t selectedBlockIndex;
	if (level == blockLevel - 1) {
		if (getBit(blockLevel, 0) == false) {
			throw std::exception("QVkMemoryManager::allocateMemory error : out of memory");
		}
		selectedBlockIndex = 0;
	}
	uint32_t currentLevel;
	MEMORY_BLOCK_INDEX offset = 0;
	for (currentLevel = level; currentLevel < blockLevel; currentLevel++) {
		bool found = false;
		for (auto wordOffset = wordOffsetsPerLevel[currentLevel]; wordOffset < wordOffsetsPerLevel[currentLevel + 1]; wordOffset++) {
			auto mask = bitMasks[wordOffset];
			if (mask != 0) {
				for (offset = 0; offset < sizeof(decltype(bitMasks.front())); offset++) {
					if ((mask >> offset) & 1)
						break;
				}
				found = true;
				break;
			}
		}
		if (found)
			break;
	}
	if (blockLevel == currentLevel) {
		throw std::exception("QVkMemoryManager::allocateMemory error : cannot find block");
	}

	resetBit(currentLevel, offset);
	while (currentLevel > level) {
		currentLevel--;
		offset = (offset << 1);
		setBit(currentLevel, offset + 1);
	}
	resetBit(currentLevel, offset);
	return (((MEMORY_BLOCK_INDEX)1 << level) + offset);
}

void QVkMemoryManager::__freeBlock(MEMORY_BLOCK_INDEX blockOffset) {
	uint32_t currentLevel = 0;
	for (currentLevel = 0; currentLevel < blockLevel; currentLevel++) {
		if (blockOffset & 1) {
			if (!getBit(currentLevel, blockOffset - 1)) {
				setBit(currentLevel, blockOffset);
				break;
			}
			resetBit(currentLevel, blockOffset - 1);
		}
		else {
			if (!getBit(currentLevel, blockOffset + 1)) {
				setBit(currentLevel, blockOffset);
				break;
			}
			resetBit(currentLevel, blockOffset + 1);
		}
		blockOffset >>= 1;
		if (currentLevel + 1 == blockLevel) {
			setBit(currentLevel, 0);
			break;
		}
	}
}