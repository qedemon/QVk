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

#include "QVkBuffer.h"
QVkBuffer* QVkDevice::createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, bool mappable) {
	QVkBuffer* pBuffer = new QVkBuffer(this, size, usage);
	VkMemoryRequirements memoryRequirements;
	pBuffer->getMemoryRequirement(&memoryRequirements);
	VkMemoryPropertyFlags preferedProperty, requiredProperty;
	requiredProperty = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
	preferedProperty = requiredProperty;
	if (mappable) {
		requiredProperty = requiredProperty | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;
		preferedProperty = requiredProperty | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
	}
	pMemoryPool->allocateMemory(memoryRequirements.memoryTypeBits, preferedProperty, requiredProperty, memoryRequirements.size, memoryRequirements.alignment);
	return pBuffer;
}
