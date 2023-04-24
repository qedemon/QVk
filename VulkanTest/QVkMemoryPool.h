#pragma once
#include "QVkMemoryManager.h"
namespace QVk {
	typedef struct QVkDeviceMemoryAllocation {
		QVkMemoryManager* pMemory;
		VkDeviceSize offset;
	}QVkDeviceMemoryAllocation;
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
		QVkDeviceMemoryAllocation allocateMemory(uint32_t memoryType, VkDeviceSize size, VkDeviceSize alignment);
		QVkDeviceMemoryAllocation allocateMemory(uint32_t allowedMemoryTypes, VkMemoryPropertyFlags preferedProperty, VkMemoryPropertyFlags requiredProperty, VkDeviceSize size, VkDeviceSize alignment);
	};
}

