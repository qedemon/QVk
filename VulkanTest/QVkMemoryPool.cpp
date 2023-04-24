#include "QVkMemoryPool.h"
#include "QVkDevice.h"

using namespace QVk;

QVkMemoryPool::QVkMemoryPool(QVkDevice* pDevice) : QVkDeviceDependent(pDevice) {
	vkGetPhysicalDeviceMemoryProperties(this->pDevice->getPhysicalDevice(), &memoryProperties);
}

void QVkMemoryPool::destroyVkResource() {
	for (auto manager : memoryManagers) {
		manager->destroy();
	}
	memoryManagers.clear();
}

std::string QVkMemoryPool::getTypeName() {
	return "Memory Pool";
}

QVkDeviceMemoryAllocation QVkMemoryPool::allocateMemory(uint32_t memoryType, VkDeviceSize size, VkDeviceSize alignment) {
	std::optional<QVkMemoryManager*> selectedMemory;
	for (auto memory : memoryManagers) {
		if (memory->getMemoryType() == memoryType) {
			selectedMemory = memory;
		}
	}
	if (!selectedMemory.has_value()) {
		selectedMemory = this->pDevice->createMemory(memoryType);
		memoryManagers.push_back(selectedMemory.value());
	}
	QVkDeviceMemoryAllocation allocation;
	allocation.pMemory = *selectedMemory;
	allocation.offset = selectedMemory.value()->allocateAlignedMemory(size, alignment);
	return allocation;
}

QVkDeviceMemoryAllocation QVkMemoryPool::allocateMemory(uint32_t allowedMemoryTypes, VkMemoryPropertyFlags preferedProperty, VkMemoryPropertyFlags requiredProperty, VkDeviceSize size, VkDeviceSize alignment) {
	std::optional<uint32_t> selectedMemoryType;
	for (uint32_t i = 0; i < memoryProperties.memoryTypeCount; i++) {
		if ((allowedMemoryTypes >> i) & 1) {
			if ((memoryProperties.memoryTypes[i].propertyFlags & preferedProperty) == preferedProperty) {
				selectedMemoryType = i;
				break;
			}
		}
	}
	if (!selectedMemoryType.has_value()) {
		for (uint32_t i = 0; i < memoryProperties.memoryTypeCount; i++) {
			if ((allowedMemoryTypes >> i) & 1) {
				if ((memoryProperties.memoryTypes[i].propertyFlags & requiredProperty) == requiredProperty) {
					break;
				}
			}
		}
	}
	if (!selectedMemoryType.has_value()) {
		throw std::exception("QVkMemoryPool::allocateMemory error : cannot find proper memory type");
	}
	return allocateMemory(selectedMemoryType.value(), size, alignment);
}