#include "QVkApp.h"
#include "AppInfo.h"
#include <iostream>

using namespace QVk;

#define BIT_CHECK(x, y) (((x&y)!=0)?( #y "|"):"")

QVkApp::QVkApp() : device() {
	this->instance = VK_NULL_HANDLE;
	this->debugMessenger = VK_NULL_HANDLE;
	this->window = nullptr;
}

bool QVkApp::initWindow() {
	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	this->window = glfwCreateWindow(WIDTH, HEIGHT, QEDEMON_APP_NAME, nullptr, nullptr);
	return true;
}

bool QVkApp::checkValidationLayerSupport() {

	uint32_t layerCount;
	vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
	std::vector<VkLayerProperties> layers(layerCount);
	vkEnumerateInstanceLayerProperties(&layerCount, layers.data());

	for (const auto& requiredLayer : validationLayers) {
		bool found = false;
		for (const auto& layer : layers) {
			if (strcmp(requiredLayer, layer.layerName) == 0) {
				found = true;
				break;
			}
		}
		if (!found)
			return false;
	}
	return true;
}


VKAPI_ATTR VkBool32 VKAPI_CALL QVkApp::debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData) {
	if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
		std::cerr << "validation layer: ";
		switch (messageSeverity) {
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
			std::cerr << "[INFO] ";
			break;
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
			std::cerr << "[WARNING] ";
			break;
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
			std::cerr << "[ERROR] ";
			break;
		}
		std::cerr<< pCallbackData->pMessage << std::endl;
	}
	return VK_FALSE;
}

std::vector<const char*> QVkApp::getRequiredExtensions() {
	uint32_t glfwExtensionCount = 0;
	const char** glfwExtensions;
	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
	std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);
	if (enableValidationLayers) {
		extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	}
	return extensions;
}

VkResult QVkApp::createInstance() {

	VkResult result = VK_SUCCESS;
	VkApplicationInfo appInfo = {};
	VkInstanceCreateInfo instanceInfo = {};

	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pNext = nullptr;
	appInfo.applicationVersion = VK_MAKE_API_VERSION(1, QEDEMON_APP_VERSION_MAYJOR, QEDEMON_APP_VERSION_MINOR, 0);
	appInfo.pApplicationName = QEDEMON_APP_NAME;
	appInfo.engineVersion = VK_MAKE_API_VERSION(1, QEDEMON_ENGINE_VERSION_MAYJOR, QEDEMON_ENGINE_VERSION_MINOR, 0);
	appInfo.pEngineName = QEDEMON_ENGINE_NAME;
	appInfo.apiVersion = VK_MAKE_API_VERSION(1, QEDEMON_API_VERSION_MAYJOR, QEDEMON_API_VERSION_MINOR, 0);

	instanceInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	instanceInfo.pNext = nullptr;
	instanceInfo.pApplicationInfo = &appInfo;
	instanceInfo.enabledLayerCount = 0;
	instanceInfo.ppEnabledLayerNames = nullptr;
	if (enableValidationLayers) {
		if (checkValidationLayerSupport()) {
			instanceInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
			instanceInfo.ppEnabledLayerNames = validationLayers.data();
		}
	}
	auto extensions = getRequiredExtensions();
	instanceInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
	instanceInfo.ppEnabledExtensionNames = extensions.data();

  	result = vkCreateInstance(&instanceInfo, nullptr, &this->instance);

	if (result != VK_SUCCESS)
		return result;

	uint32_t physicalDeviceCount;
	result = vkEnumeratePhysicalDevices(this->instance, &physicalDeviceCount, nullptr);
	if (physicalDeviceCount <= 0) {
		throw("There is no physical device in Vulkan instance.");
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

static VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger) {
	auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
	if (func != nullptr) {
		return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
	}
	else {
		return VK_ERROR_EXTENSION_NOT_PRESENT;
	}
}

static void DestroyDebugUtilsMessagerEXT(VkInstance instance, VkDebugUtilsMessengerEXT messenger, const VkAllocationCallbacks* pAllocator) {
	auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
	if (func != nullptr) {
		return func(instance, messenger, pAllocator);
	}
}

void QVkApp::setupDebugMessenger() {
	if (!enableValidationLayers)
		return;
	VkDebugUtilsMessengerCreateInfoEXT debugMessengerInfo = {};
	debugMessengerInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	debugMessengerInfo.pNext = nullptr;
	debugMessengerInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	debugMessengerInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_DEVICE_ADDRESS_BINDING_BIT_EXT;
	debugMessengerInfo.pfnUserCallback = QVkApp::debugCallback;
	debugMessengerInfo.pUserData = nullptr;
	if (CreateDebugUtilsMessengerEXT(instance, &debugMessengerInfo, nullptr, &debugMessenger) != VK_SUCCESS) {
		throw std::runtime_error("failed to create instance.");
	}
}


VkResult QVkApp::initVulkan() {
	VkResult result=VK_SUCCESS;
	result = createInstance();
	if (result != VK_SUCCESS) {
		throw std::runtime_error("Create Vulkan Instance error");
	}
	setupDebugMessenger();

#ifdef _DEBUG
	int i = 0;
	for (auto iter = this->physicalDevices.begin(); iter != this->physicalDevices.end(); ++iter) {
		std::cout << this->physicalDeviceProperties[i].deviceName << std::endl;
		VkPhysicalDeviceMemoryProperties phyDevMemProp;
		vkGetPhysicalDeviceMemoryProperties(*iter, &phyDevMemProp);
		std::cout << "Memory types" << std::endl;
		for (size_t j = 0; j < phyDevMemProp.memoryTypeCount; j++) {
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
#endif

	VkPhysicalDeviceFeatures requiredFeatures = {};
	requiredFeatures.geometryShader = VK_TRUE;
	const std::vector<const char*> requiredExtensions = {
	VK_KHR_SWAPCHAIN_EXTENSION_NAME
	};
	this->device.setupPhysicalDevice(physicalDevices, physicalDeviceProperties, requiredFeatures, requiredExtensions, enableValidationLayers ? validationLayers : std::vector<const char*>());
	this->device.createDevice();

	pDeviceMemory = new QVkMemoryManager(this->device.getLogicalDevice(), this->device.getPhysicalDevice(), 0);
	pDeviceMemory->allocateMemory((1<<10) + 1);
	pDeviceMemory->allocateMemory( 1);
	return result;
}

void QVkApp::cleanupVulkan() {
	delete pDeviceMemory;
	this->device.destroyDevice();
	DestroyDebugUtilsMessagerEXT(this->instance, this->debugMessenger, nullptr);
	vkDestroyInstance(this->instance, nullptr);
}

void QVkApp::init() {
	initWindow();
	initVulkan();
}

void QVkApp::mainLoop() {
	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
	}
}

void QVkApp::destroy() {
	cleanupVulkan();
	glfwTerminate();
}
