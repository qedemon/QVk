#include "QVkApp.h"
#include "AppInfo.h"
#include <iostream>

using namespace QVk;

#define BIT_CHECK(x, y) (((x&y)!=0)?( #y "|"):"")

QVkApp::QVkApp() : device() {
	this->instance = VK_NULL_HANDLE;
}

VkResult QVkApp::createInstance() {
	VkResult result = VK_SUCCESS;
	VkApplicationInfo appInfo = {};
	VkInstanceCreateInfo instanceInfo = {};

	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pNext = nullptr;
	appInfo.applicationVersion = QEDEMON_APP_VERSION;
	appInfo.pApplicationName = QEDEMON_APP_NAME;
	appInfo.engineVersion = QEDEMON_ENGINE_VERSION;
	appInfo.pEngineName = QEDEMON_ENGINE_NAME;
	appInfo.apiVersion = VK_MAKE_API_VERSION(1, QEDEMON_API_VERSION_MAYJOR, QEDEMON_API_VERSION_MINOR, 0);

	instanceInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	instanceInfo.pNext = nullptr;
	instanceInfo.pApplicationInfo = &appInfo;
	instanceInfo.enabledLayerCount = 0;
	instanceInfo.ppEnabledLayerNames = nullptr;
	instanceInfo.enabledExtensionCount = 0;
	instanceInfo.ppEnabledExtensionNames = nullptr;

	result = vkCreateInstance(&instanceInfo, nullptr, &this->instance);

	if (result != VK_SUCCESS)
		return result;

	uint32_t physicalDeviceCount;
	result = vkEnumeratePhysicalDevices(this->instance, &physicalDeviceCount, nullptr);
	if (physicalDeviceCount <= 0) {
		std::cerr << "There is no physical device in Vulkan instance." << std::endl;
		return result;
	}
	this->physicalDevices.resize(physicalDeviceCount);
	this->physicalDeviceProperties.resize(physicalDeviceCount);
	result = vkEnumeratePhysicalDevices(this->instance, &physicalDeviceCount, physicalDevices.data());
	int i = 0;
	for (auto iter = this->physicalDevices.begin(); iter != this->physicalDevices.end(); ++iter) {
		vkGetPhysicalDeviceProperties(*iter, this->physicalDeviceProperties.data() + i);
		i++;
	}

	return result;
}


VkResult QVkApp::init() {
	VkResult result=VK_SUCCESS;
	result = createInstance();
	if (result != VK_SUCCESS) {
		std::cerr << "Create Vulkan Instance error" << std::endl;
		return result;
	}
	
	int i = 0;
	for (auto iter = this->physicalDevices.begin(); iter != this->physicalDevices.end(); ++iter) {
		std::cout << this->physicalDeviceProperties[i].deviceName << std::endl;
		VkPhysicalDeviceMemoryProperties phyDevMemProp;
		vkGetPhysicalDeviceMemoryProperties(*iter, &phyDevMemProp);
		std::cout << "Memory types" << std::endl;
		for (int j = 0; j < phyDevMemProp.memoryTypeCount; j++) {
			std::cerr << j << " : "
				<< phyDevMemProp.memoryTypes[j].propertyFlags << std::endl
				<< BIT_CHECK(phyDevMemProp.memoryTypes[j].propertyFlags, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)
				<< BIT_CHECK(phyDevMemProp.memoryTypes[j].propertyFlags, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT)
				<< BIT_CHECK(phyDevMemProp.memoryTypes[j].propertyFlags, VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)
				<< BIT_CHECK(phyDevMemProp.memoryTypes[j].propertyFlags, VK_MEMORY_PROPERTY_HOST_CACHED_BIT)
				<< BIT_CHECK(phyDevMemProp.memoryTypes[j].propertyFlags, VK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT)
				<< std::endl;
		}
		i++;
	}

	this->device.setupPhysicalDevice(physicalDevices, physicalDeviceProperties);

	return result;
}

void QVkApp::destroy() {
	vkDestroyInstance(this->instance, nullptr);
}