#include "QVkMemory.h"
#include <algorithm>
using namespace QVk;

QVkMemory::QVkMemory(VkDevice logicalDevice, VkPhysicalDevice physicalDevice) : 
	logicalDevice(logicalDevice), physicalDevice(physicalDevice), deviceMemory(VK_NULL_HANDLE){

}

bool QVkMemory::selectMemoryTypeAndHeap(const std::vector<VkMemoryPropertyFlagBits>& whiteList, const std::vector<VkMemoryPropertyFlagBits>& blackList) {
	VkPhysicalDeviceMemoryProperties memoryProperties;
	vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memoryProperties);
	std::vector<int> typePoint(memoryProperties.memoryTypeCount);
	for (auto point : typePoint) {
		point = 0;
	}
	for (int i=0; i < memoryProperties.memoryTypeCount; i++) {
		auto propertyFlag = memoryProperties.memoryTypes[i].propertyFlags;
		for (auto white : whiteList) {
			if (white & propertyFlag) {
				typePoint[i]++;
			}
		}
		for (auto black : blackList) {
			if (black & propertyFlag) {
				typePoint[i]--;
			}
		}
	}
	memoryTypeIndex = std::max_element(typePoint.begin(), typePoint.end()) - typePoint.begin();
	heapIndex = memoryProperties.memoryTypes[memoryTypeIndex].heapIndex;
	return true;
}

VkResult QVkMemory::allocateMemory(VkDeviceSize allocateSize) {
	VkMemoryAllocateInfo allocateInfo = {};
	allocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocateInfo.pNext = nullptr;
	allocateInfo.allocationSize = allocateSize;
	allocateInfo.memoryTypeIndex = memoryTypeIndex;
	return vkAllocateMemory(logicalDevice, &allocateInfo, nullptr, &deviceMemory);
}