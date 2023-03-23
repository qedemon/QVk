#include "QVkDevice.h"
#include <iostream>
using namespace QVk;

QVkDevice::QVkDevice() {
	this->device = VK_NULL_HANDLE;
	this->physicalDevice = VK_NULL_HANDLE;
}

bool QVkDevice::setupPhysicalDevice(std::vector<VkPhysicalDevice>& phyDevs, std::vector<VkPhysicalDeviceProperties>& phyDevProps) {
	this->selectedQueueFamilies = { UINT32_MAX, UINT32_MAX, UINT32_MAX };

	for (auto iter = phyDevs.begin(); iter != phyDevs.end(); ++iter) {
		VkPhysicalDeviceFeatures deviceFeatures;
		vkGetPhysicalDeviceFeatures(*iter, &deviceFeatures);
		if (!deviceFeatures.geometryShader)
			continue;

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
	if (!this->selectedQueueFamilies.graphicsQueueFamilyIndex.has_value()) {
		return false;
	}
	std::cerr << "Graphics " << selectedQueueFamilies.graphicsQueueFamilyIndex.value() << std::endl
		<< "Compute " << selectedQueueFamilies.computeQueueFamiliyIndex.value() << std::endl
		<< "Transfer " << selectedQueueFamilies.transferQueueFamiliyIndex.value() << std::endl;
	return true;
}

VkResult QVkDevice::createDevice(VkPhysicalDeviceFeatures deviceFeatures, const std::vector<const char*>& requiredExtensions, const std::vector<const char*>& requiredLayers) {
	VkDeviceQueueCreateInfo queueInfo = {};
	queueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	queueInfo.pNext = nullptr;
	queueInfo.queueFamilyIndex = selectedQueueFamilies.graphicsQueueFamilyIndex.value();
	queueInfo.queueCount = 1;
	float queuePriority = 1.0f;
	queueInfo.pQueuePriorities = &queuePriority;
	
	VkDeviceCreateInfo deviceInfo = {};
	deviceInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	deviceInfo.pNext = nullptr;
	deviceInfo.pEnabledFeatures = &deviceFeatures;
	deviceInfo.enabledExtensionCount = static_cast<uint32_t>(requiredExtensions.size());
	deviceInfo.ppEnabledExtensionNames = requiredExtensions.data();
	deviceInfo.enabledLayerCount = static_cast<uint32_t>(requiredLayers.size());
	deviceInfo.ppEnabledLayerNames = requiredLayers.data();
	deviceInfo.queueCreateInfoCount = 1;
	deviceInfo.pQueueCreateInfos = &queueInfo;

	if (vkCreateDevice(physicalDevice, &deviceInfo, nullptr, &device) != VK_SUCCESS) {
		throw std::runtime_error("create logical device failed.");
	}

	vkGetDeviceQueue(device, selectedQueueFamilies.graphicsQueueFamilyIndex.value(), 1, &graphicsQueue);

	return VK_SUCCESS;
}

void QVkDevice::destroyDevice() {
	if (this->device != nullptr) {
		vkDestroyDevice(this->device, nullptr);
	}
}