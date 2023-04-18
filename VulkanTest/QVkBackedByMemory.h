#pragma once
#include "QVkMemory.h"
#include "QVkDeviceDependent.h"
namespace QVk {
	class QVkBackedByMemory : public QVkDeviceDependent
	{
	protected:
		QVkMemory* pBackMemory;
		bool mappable;
		VkDeviceSize offset, size;
	protected:
		QVkBackedByMemory(QVkDevice* pDevice, QVkMemory* pBackMemory, VkDeviceSize offset, VkDeviceSize size, bool mappable);
		void* map();
		void unmap();
	};
}
