#include "QVkDevice.h"
using namespace QVk;

#include "QVkMemoryManager.h"
QVkMemoryManager* QVkDevice::createMemory(uint32_t memoryIndex) {
	return new QVkMemoryManager(this, memoryIndex);
}

#include "QVkMemoryPool.h"
QVkMemoryPool* QVkDevice::createMemoryPool() {
	return new QVkMemoryPool(this);
}