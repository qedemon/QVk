#pragma once
#include <vulkan/vulkan.h>
#include <vector>
#include <optional>
namespace QVk {
	class QVkMemory
	{
	private:
		VkDevice logicalDevice;
		VkPhysicalDevice physicalDevice;
		std::optional<uint32_t> memoryTypeIndex;
		std::optional<VkMemoryType> memoryType;
		std::optional<VkDeviceSize> allocatedSize;
		//VkMemoryPropertyFlags memoryProperty;
		//uint32_t heapIndex;
		VkDeviceMemory deviceMemory;
	public:
		QVkMemory(VkDevice logicalDevice, VkPhysicalDevice physicalDevice);
		~QVkMemory();
		//bool selectMemoryTypeAndHeap(const std::vector<VkMemoryPropertyFlagBits>& whiteList, const std::vector<VkMemoryPropertyFlagBits>& blackList);
		VkResult allocateMemory(uint32_t memoryTypeIndex, VkDeviceSize allocationSize);
		void freeMemory();
		void* mapMemory(VkDeviceSize offset, VkDeviceSize size);
		void unmapMemory();
		bool flushMemory(VkDeviceSize offset, VkDeviceSize size);
		bool flushMemory(const std::vector<VkDeviceSize>& offset, const std::vector<VkDeviceSize>& size);
		bool invalidateMemory(VkDeviceSize offset, VkDeviceSize size);
		bool invalidateMemory(const std::vector<VkDeviceSize>& offset, const std::vector<VkDeviceSize>& size);
	};
}

