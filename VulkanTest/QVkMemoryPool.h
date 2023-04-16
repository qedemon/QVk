#pragma once
#include "QVkMemoryManager.h"
namespace QVk {
	class QVkMemoryPool
	{
	private:
		VkDevice logicalDevice;
		VkPhysicalDevice physicalDevice;
		std::vector<QVkMemoryManager*> memoryManagers;
	public:
		QVkMemoryPool(VkDevice logicalDevice, VkPhysicalDevice physicalDevice);
		~QVkMemoryPool();
	};
}

