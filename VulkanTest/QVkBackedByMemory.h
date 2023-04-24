#pragma once
#include "QVkMemoryManager.h"
#include "QVkDeviceDependent.h"
namespace QVk {
	class QVkBackedByMemory : public QVkDeviceDependent
	{
	protected:
		QVkMemoryManager* pBackMemory;
		bool mappable;
		std::optional<VkDeviceSize> offset, size;
	private:
		virtual bool _bindVkMemory(QVkMemoryManager* pBackMemory, VkDeviceSize offset) = 0;
	protected:
		QVkBackedByMemory(QVkDevice* pDevice);
		bool bindMemory(QVkMemoryManager* pBackMemory, VkDeviceSize offset, VkDeviceSize size, bool mappable);
		VkDeviceMemory getVkDeviceMemory();
	public:
		virtual void getMemoryRequirement(VkMemoryRequirements* pMemoryRequirement) = 0;
		void* map(VkDeviceSize offset, VkDeviceSize size);
		void unmap();
	};
}
