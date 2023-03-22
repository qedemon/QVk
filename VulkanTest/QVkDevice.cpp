#include "QVkDevice.h"
#include <iostream>
using namespace QVk;

QVkDevice::QVkDevice(){
	this->device = VK_NULL_HANDLE;
	this->physicalDevice = VK_NULL_HANDLE;
	this->selectedQueueFamilies = {UINT32_MAX, UINT32_MAX, UINT32_MAX};
}

bool QVkDevice::setupPhysicalDevice(std::vector<VkPhysicalDevice>& phyDevs, std::vector<VkPhysicalDeviceProperties>& phyDevProps) {
	this->selectedQueueFamilies = { UINT32_MAX, UINT32_MAX, UINT32_MAX };

	for (auto iter = phyDevs.begin(); iter != phyDevs.end(); ++iter) {
		std::vector<VkQueueFamilyProperties> vQueueFamilyProperties;
		uint32_t uQueueFamilyPropertiesCount;
		vkGetPhysicalDeviceQueueFamilyProperties(*iter, &uQueueFamilyPropertiesCount, nullptr);
		vQueueFamilyProperties.resize(uQueueFamilyPropertiesCount);
		vkGetPhysicalDeviceQueueFamilyProperties(*iter, &uQueueFamilyPropertiesCount, vQueueFamilyProperties.data());
		int i = 0;
		auto queueFamilyPropsIter = vQueueFamilyProperties.begin();
		for (auto queueFamilyPropsIter = vQueueFamilyProperties.begin(); queueFamilyPropsIter != vQueueFamilyProperties.end(); ++queueFamilyPropsIter) {
			if ((queueFamilyPropsIter->queueFlags & VK_QUEUE_GRAPHICS_BIT)) {
				this->selectedQueueFamilies.graphicsQueueFamilyIndex = i;
				this->physicalDevice = *iter;
				break;
			}
			i++;
		}
		if (i == vQueueFamilyProperties.size()) {
			continue;
		}

		i = 0;
		for (auto queueFamilyPropsIter = vQueueFamilyProperties.begin(); queueFamilyPropsIter != vQueueFamilyProperties.end(); ++queueFamilyPropsIter) {
			if ((queueFamilyPropsIter->queueFlags & VK_QUEUE_TRANSFER_BIT)) {
				this->selectedQueueFamilies.transferQueueFamiliyIndex = i;
				break;
			}
			i++;
		}

		i = 0;
		for (auto queueFamilyPropsIter = vQueueFamilyProperties.begin(); queueFamilyPropsIter != vQueueFamilyProperties.end(); ++queueFamilyPropsIter) {
			if ((queueFamilyPropsIter->queueFlags & VK_QUEUE_COMPUTE_BIT)) {
				this->selectedQueueFamilies.computeQueueFamiliyIndex = i;
				break;
			}
			i++;
		}
	}
	if (this->selectedQueueFamilies.graphicsQueueFamilyIndex == UINT32_MAX) {
		return false;
	}
	std::cerr << "Graphics " << selectedQueueFamilies.graphicsQueueFamilyIndex << std::endl
		<< "Compute " << selectedQueueFamilies.computeQueueFamiliyIndex << std::endl
		<< "Transfer " << selectedQueueFamilies.transferQueueFamiliyIndex << std::endl;

}