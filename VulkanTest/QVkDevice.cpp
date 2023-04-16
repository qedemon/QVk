#include "QVkDevice.h"
#include <iostream>
#include <map>
using namespace QVk;

QVkDevice::QVkDevice() {
	this->device = VK_NULL_HANDLE;
	this->physicalDevice = VK_NULL_HANDLE;
}

bool checkDeviceFeature(VkPhysicalDevice physicalDevice, const VkPhysicalDeviceFeatures& requeiredFeature) {
	VkPhysicalDeviceFeatures supportedFeatures;
	vkGetPhysicalDeviceFeatures(physicalDevice, &supportedFeatures);
	for (size_t i = 0; i < sizeof(VkPhysicalDeviceFeatures)/sizeof(VkBool32); i++) {
		if ((((VkBool32*)&requeiredFeature)[i] == VK_TRUE) && (((VkBool32*)&supportedFeatures)[i] == VK_FALSE)) {
			return false;
		}
	}
	return true;
}

bool checkDeviceExtension(VkPhysicalDevice physicalDevice, const std::vector<const char*>& requiredExtensions) {
	uint32_t supportedExtensionCount = 0;
	std::vector<VkExtensionProperties> supportedExtensions;
	vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &supportedExtensionCount, nullptr);
	supportedExtensions.resize(supportedExtensionCount);
	vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &supportedExtensionCount, supportedExtensions.data());
	for (const auto& requiredExtension : requiredExtensions) {
		bool found = false;
		for (const auto& supportedExtension : supportedExtensions) {
			if (!strcmp(requiredExtension, supportedExtension.extensionName)) {
				found = true;
				break;
			}
		}
		if (!found) {
			return false;
		}
	}
	return true;
}

bool checkDeviceLayer(VkPhysicalDevice physicalDevice, const std::vector<const char*>& requiredLayers) {
	uint32_t supportedLayerCount = 0;
	std::vector<VkLayerProperties> supportedLayers;
	vkEnumerateDeviceLayerProperties(physicalDevice, &supportedLayerCount, nullptr);
	supportedLayers.resize(supportedLayerCount);
	vkEnumerateDeviceLayerProperties(physicalDevice, &supportedLayerCount, supportedLayers.data());
	for (const auto& requiredLayer : requiredLayers) {
		bool found = false;
		for (const auto& supportedLayer : supportedLayers) {
			if (!strcmp(requiredLayer, supportedLayer.layerName)) {
				found = true;
				break;
			}
		}
		if (!found) {
			return false;
		}
	}
	return true;
}

bool QVkDevice::setupPhysicalDevice(std::vector<VkPhysicalDevice>& phyDevs, std::vector<VkPhysicalDeviceProperties>& phyDevProps, VkPhysicalDeviceFeatures requiredDeviceFeatures, const std::vector<const char*>& requiredExtensions, const std::vector<const char*>& requiredLayers) {
	this->selectedQueueFamilies = { UINT32_MAX, UINT32_MAX, UINT32_MAX };

	for (const auto& physicalDevice : phyDevs) {
		if (!checkDeviceFeature(physicalDevice, requiredDeviceFeatures))
			continue;
		this->deviceFeatures = requiredDeviceFeatures;
		
		if (!checkDeviceExtension(physicalDevice, requiredExtensions))
			continue;
		this->deviceExtensions.resize(requiredExtensions.size());
		std::copy(requiredExtensions.begin(), requiredExtensions.end(), this->deviceExtensions.begin());

		if (!checkDeviceLayer(physicalDevice, requiredLayers))
			continue;
		this->deviceLayers.resize(requiredLayers.size());
		std::copy(requiredLayers.begin(), requiredLayers.end(), this->deviceLayers.begin());

		std::vector<VkQueueFamilyProperties> vQueueFamilyProperties;
		uint32_t uQueueFamilyPropertiesCount;
		vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &uQueueFamilyPropertiesCount, nullptr);
		vQueueFamilyProperties.resize(uQueueFamilyPropertiesCount);
		vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &uQueueFamilyPropertiesCount, vQueueFamilyProperties.data());
		int i = 0;
		auto queueFamilyPropsIter = vQueueFamilyProperties.begin();
		for (auto queueFamilyPropsIter = vQueueFamilyProperties.begin(); queueFamilyPropsIter != vQueueFamilyProperties.end(); ++queueFamilyPropsIter) {
			if ((queueFamilyPropsIter->queueFlags & VK_QUEUE_GRAPHICS_BIT)) {
				this->selectedQueueFamilies.graphicsQueueFamilyIndex = i;
				this->physicalDevice = physicalDevice;
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

static void countPerQueueFailiyindices(std::map<uint32_t, uint32_t> &queueFamilies, uint32_t queueFamiliindex) {
	auto queueFamilyIndex = queueFamilies.find(queueFamiliindex);
	if (queueFamilyIndex == queueFamilies.end()) {
		queueFamilies.insert(std::pair(queueFamiliindex, (uint32_t)1));
	}
	else {
		queueFamilyIndex->second++;
	}
}

VkResult QVkDevice::createDevice() {
	std::vector<VkDeviceQueueCreateInfo> queueInfos;
	std::map<uint32_t, uint32_t> queueFamilies;
	std::vector<float> priorities;
	
	if (selectedQueueFamilies.graphicsQueueFamilyIndex.has_value())
		countPerQueueFailiyindices(queueFamilies, selectedQueueFamilies.graphicsQueueFamilyIndex.value());
	if(selectedQueueFamilies.computeQueueFamiliyIndex.has_value())
		countPerQueueFailiyindices(queueFamilies, selectedQueueFamilies.computeQueueFamiliyIndex.value());
	if (selectedQueueFamilies.transferQueueFamiliyIndex.has_value())
		countPerQueueFailiyindices(queueFamilies, selectedQueueFamilies.transferQueueFamiliyIndex.value());

	for (auto queueFamilyIndexCount : queueFamilies) {
		VkDeviceQueueCreateInfo queueCreateInfo = {};
		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.queueFamilyIndex = queueFamilyIndexCount.first;
		queueCreateInfo.queueCount = 1;
		for (size_t i = 0; i < queueCreateInfo.queueCount; i++)
			priorities.push_back(1.0f);
		queueCreateInfo.pQueuePriorities = &priorities.back();
		queueInfos.push_back(queueCreateInfo);
	}

	VkDeviceCreateInfo deviceInfo = {};
	deviceInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	deviceInfo.pNext = nullptr;
	deviceInfo.pEnabledFeatures = &deviceFeatures;
	deviceInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
	deviceInfo.ppEnabledExtensionNames = deviceExtensions.data();
	deviceInfo.enabledLayerCount = static_cast<uint32_t>(deviceLayers.size());
	deviceInfo.ppEnabledLayerNames = deviceLayers.data();
	deviceInfo.queueCreateInfoCount = static_cast<uint32_t>(queueInfos.size());
	deviceInfo.pQueueCreateInfos = queueInfos.data();

	if (vkCreateDevice(physicalDevice, &deviceInfo, nullptr, &device) != VK_SUCCESS) {
		throw std::runtime_error("create logical device failed.");
	}

	vkGetDeviceQueue(device, selectedQueueFamilies.graphicsQueueFamilyIndex.value(), 0, &graphicsQueue);
	if(selectedQueueFamilies.computeQueueFamiliyIndex.has_value())
		vkGetDeviceQueue(device, selectedQueueFamilies.computeQueueFamiliyIndex.value(), 0, &computeQueue);
	if(selectedQueueFamilies.transferQueueFamiliyIndex.has_value())
		vkGetDeviceQueue(device, selectedQueueFamilies.transferQueueFamiliyIndex.value(), 0, &transferQueue);

	return VK_SUCCESS;
}

void QVkDevice::destroyDevice() {
	if (this->device != nullptr) {
		vkDeviceWaitIdle(this->device);
		vkDestroyDevice(this->device, nullptr);
	}
}