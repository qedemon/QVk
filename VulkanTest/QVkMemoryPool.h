#pragma once
#include "QVkMemoryManager.h"
namespace QVk {
	class QVkMemoryPool :QVkDeviceDependent
	{
	private:
		VkPhysicalDeviceMemoryProperties memoryProperties;
		std::vector<QVkMemoryManager*> memoryManagers;
	protected:
		void destroyVkResource();
		QVkMemoryPool(QVkDevice* pDevice);
	public:
		std::string getTypeName();
		friend class QVkDevice;
	public:
		VkDeviceSize allocateMemory(uint32_t memoryType, VkDeviceSize size, VkDeviceSize alignment);
		VkDeviceSize allocateMemory(uint32_t allowedMemoryTypes, VkMemoryPropertyFlags preferedProperty, VkMemoryPropertyFlags requiredProperty, VkDeviceSize size, VkDeviceSize alignment);
	};
}

