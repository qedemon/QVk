#include "QVkDevice.h"
#include "QVkDeviceQueue.h"
#include <iostream>
#include <map>
using namespace QVk;

QVkDevice::QVkDevice() {
	this->instance = VK_NULL_HANDLE;
	this->device = VK_NULL_HANDLE;
	this->physicalDevice = VK_NULL_HANDLE;
	this->deviceFeatures = {};
}

/*bool QVkDevice::setupPhysicalDevice(VkInstance instance, VkPhysicalDeviceFeatures requiredDeviceFeatures, const std::vector<const char*>& requiredExtensions, const std::vector<const char*>& requiredLayers, VkSurfaceKHR surface) {
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
}*/

static void countPerQueueFailiyindices(std::map<uint32_t, uint32_t> &queueFamilies, uint32_t queueFamilyindex) {
	auto queueFamilyIndex = queueFamilies.find(queueFamilyindex);
	if (queueFamilyIndex == queueFamilies.end()) {
		queueFamilies.insert(std::pair(queueFamilyindex, (uint32_t)1));
	}
	else {
		queueFamilyIndex->second++;
	}
}

VkResult QVkDevice::createDevice(VkInstance instance, VkPhysicalDevice physicalDevice, VkPhysicalDeviceFeatures requiredDeviceFeatures, const std::vector<const char*>& requiredExtensions, const std::vector<const char*>& requiredLayers, const std::vector<uint32_t>& queueFamilyIndices, std::vector<QVkDeviceQueue*>& deviceQueues) {
	this->instance = instance;
	this->physicalDevice = physicalDevice;
	this->deviceFeatures = requiredDeviceFeatures;
	this->deviceExtensions = requiredExtensions;
	this->deviceLayers = requiredLayers;
	
	std::vector<VkDeviceQueueCreateInfo> queueInfos;
	std::map<uint32_t, uint32_t> queueFamilies;
	std::vector<float> priorities;
	
	for (auto queueFamilyIndex : queueFamilyIndices) {
		countPerQueueFailiyindices(queueFamilies, queueFamilyIndex);
	}

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

	for (auto queueFamilyIndex = queueFamilyIndices.begin(); queueFamilyIndex != queueFamilyIndices.end(); queueFamilyIndex++) {
		VkQueue queue;
		vkGetDeviceQueue(device, *queueFamilyIndex, 0, &queue);
		QVkDeviceQueue* pQueue = new QVkDeviceQueue(this, queue, *queueFamilyIndex);
		deviceQueues.push_back(pQueue);
	}
	return VK_SUCCESS;
}

void QVkDevice::destroyDevice() {
	while(this->deviceDependents.size()>0) {
		auto iter = deviceDependents.begin();
		QVkDeviceDependent* pDependent = *iter;
		pDependent->destroy();
		delete pDependent;
	}
	if (this->device != nullptr) {
		vkDeviceWaitIdle(this->device);
		vkDestroyDevice(this->device, nullptr);
	}
}

void QVkDevice::registerDeviceDependent(QVkDeviceDependent* pDependent){
	this->deviceDependents.insert(pDependent);
}

void QVkDevice::unregisterDeviceDependent(QVkDeviceDependent* pDependent) {
	this->deviceDependents.erase(pDependent);
}