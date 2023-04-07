#pragma once
#include <vulkan/vulkan.h>
#include <vector>
namespace QVk {
	class QVkMemory
	{
	private:
		VkDevice logicalDevice;
		VkPhysicalDevice physicalDevice;
		uint32_t memoryTypeIndex;
		uint32_t heapIndex;
		VkDeviceMemory deviceMemory;
	public:
		QVkMemory(VkDevice logicalDevice, VkPhysicalDevice physicalDevice);
		~QVkMemory();
		bool selectMemoryTypeAndHeap(const std::vector<VkMemoryPropertyFlagBits>& whiteList, const std::vector<VkMemoryPropertyFlagBits>& blackList);
		VkResult allocateMemory(VkDeviceSize allocationSize);
		void freeMemory();
	};
}

