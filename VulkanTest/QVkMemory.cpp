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

/*bool QVkMemory::selectMemoryTypeAndHeap(const std::vector<VkMemoryPropertyFlagBits>& whiteList, const std::vector<VkMemoryPropertyFlagBits>& blackList) {
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
	memoryProperty = memoryProperties.memoryTypes[memoryTypeIndex].propertyFlags;
	heapIndex = memoryProperties.memoryTypes[memoryTypeIndex].heapIndex;
	return true;
}
*/
VkResult QVkMemory::allocateMemory(uint32_t memoryTypeIndex, VkDeviceSize allocateSize) {

	VkMemoryAllocateInfo allocateInfo = {};
	allocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocateInfo.pNext = nullptr;
	allocateInfo.allocationSize = allocateSize;
	allocateInfo.memoryTypeIndex = memoryTypeIndex;
	VkResult result = vkAllocateMemory(logicalDevice, &allocateInfo, nullptr, &deviceMemory);
	if (result == VK_SUCCESS) {
		allocatedSize = allocateSize;
		this->memoryTypeIndex = memoryTypeIndex;
		VkPhysicalDeviceMemoryProperties memoryProperties;
		vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memoryProperties);
		this->memoryType = memoryProperties.memoryTypes[memoryTypeIndex];
	}
	return VK_SUCCESS;
}

void QVkMemory::freeMemory() {
	if(deviceMemory)
		vkFreeMemory(logicalDevice, deviceMemory, nullptr);
	deviceMemory = nullptr;
}

void* QVkMemory::mapMemory(VkDeviceSize offset, VkDeviceSize size) {
	void* memoryAddress = nullptr;
	if ((memoryType.value().propertyFlags | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) == 0) {
		throw std::exception("Not host visible memory.");
		return nullptr;
	}
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

bool QVkMemory::flushMemory(VkDeviceSize offset, VkDeviceSize size) {
	return flushMemory(std::vector<VkDeviceSize>(offset), std::vector<VkDeviceSize>(size));
}

bool QVkMemory::flushMemory(const std::vector<VkDeviceSize>& offset, const std::vector<VkDeviceSize>& size) {
	if (offset.size() == 0)
		return false;
	std::vector<VkMappedMemoryRange> mappedRanges;
	for (size_t i = 0; i < offset.size(); i++) {
		VkMappedMemoryRange mappedRange;
		mappedRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
		mappedRange.memory = this->deviceMemory;
		mappedRange.offset = offset[i];
		mappedRange.size = size[i];
		mappedRanges.push_back(mappedRange);
	}
	return (vkFlushMappedMemoryRanges(logicalDevice, static_cast<uint32_t>(mappedRanges.size()), mappedRanges.data()) == VK_SUCCESS);
}

bool QVkMemory::invalidateMemory(VkDeviceSize offset, VkDeviceSize size) {
	return invalidateMemory(std::vector<VkDeviceSize>(offset), std::vector<VkDeviceSize>(size));
}

bool QVkMemory::invalidateMemory(const std::vector<VkDeviceSize>& offset, const std::vector<VkDeviceSize>& size) {
	if (offset.size() == 0)
		return false;
	std::vector<VkMappedMemoryRange> mappedRanges;
	for (size_t i = 0; i < offset.size(); i++) {
		VkMappedMemoryRange mappedRange;
		mappedRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
		mappedRange.memory = this->deviceMemory;
		mappedRange.offset = offset[i];
		mappedRange.size = size[i];
		mappedRanges.push_back(mappedRange);
	}
	return (vkInvalidateMappedMemoryRanges(logicalDevice, static_cast<uint32_t>(mappedRanges.size()), mappedRanges.data()) == VK_SUCCESS);
}

