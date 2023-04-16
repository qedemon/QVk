#include "QVkMemoryPool.h"

using namespace QVk;

QVkMemoryPool::QVkMemoryPool(VkDevice logicalDevice, VkPhysicalDevice physicalDevice) : logicalDevice(logicalDevice), physicalDevice(physicalDevice) {

}

QVkMemoryPool::~QVkMemoryPool() {
	for (auto pMemoryManager : memoryManagers) {
		delete pMemoryManager;
	}
}