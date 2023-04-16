#pragma once
#include "QVkMemory.h"
namespace QVk {
	class QVkBackedByMemory
	{
	protected:
		QVkMemory* pBackMemory;
		bool mappable;
		VkDeviceSize offset, size;
	protected:
		QVkBackedByMemory(QVkMemory* pBackMemory, VkDeviceSize offset, VkDeviceSize size, bool mappable);
		void* map();
		void unmap();
	};
}
