#include "QVkMemory.h"
#include <algorithm>
#include <exception>

using namespace QVk;

QVkMemory::QVkMemory(VkDevice logicalDevice, VkPhysicalDevice physicalDevice) : 
	logicalDevice(logicalDevice), physicalDevice(physicalDevice), deviceMemory(VK_NULL_HANDLE){

}

QVkMemory::~QVkMemory() {
	freeMemory();
}

bool QVkMemory::selectMemoryTypeAndHeap(const std::vector<VkMemoryPropertyFlagBits>& whiteList, const std::vector<VkMemoryPropertyFlagBits>& blackList) {
	VkPhysicalDeviceMemoryProperties memoryProperties;
	vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memoryProperties);
	std::vector<int> typePoint(memoryProperties.memoryTypeCount);
	for (auto point : typePoint) {
		point = 0;
	}
	for (size_t i=0; i < memoryProperties.memoryTypeCount; i++) {
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
	memoryTypeIndex = static_cast<uint32_t>(std::max_element(typePoint.begin(), typePoint.end()) - typePoint.begin());
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

void QVkMemory::freeMemory() {
	if(deviceMemory)
		vkFreeMemory(logicalDevice, deviceMemory, nullptr);
	deviceMemory = nullptr;
}

void* QVkMemory::mapMemory(VkDeviceSize offset, VkDeviceSize size) {
	void* memoryAddress = nullptr;
	VkResult result = vkMapMemory(logicalDevice, deviceMemory, offset, size, 0, &memoryAddress);
	if (result == VK_SUCCESS) {
		return memoryAddress;
	}
	switch (result) {
	case VK_ERROR_OUT_OF_HOST_MEMORY:
		throw std::exception("out of host memory.");
		break;
	case VK_ERROR_OUT_OF_DEVICE_MEMORY:
		throw std::exception("out of device memory.");
		break;
	case VK_ERROR_MEMORY_MAP_FAILED:
		throw std::exception("memory map failed.");
	}
	return nullptr;
}