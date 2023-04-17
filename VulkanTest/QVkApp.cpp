#include "QVkApp.h"
#include "AppInfo.h"
#include <iostream>
#include "QVkDeviceQueue.h"

using namespace QVk;

#define BIT_CHECK(x, y) (((x&y)!=0)?( #y "|"):"")

QVkApp::QVkApp() : device() {
	this->instance = VK_NULL_HANDLE;
	this->debugMessenger = VK_NULL_HANDLE;
	this->window = nullptr;
	this->surface = VK_NULL_HANDLE;
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

void QVkApp::createSurface() {
	glfwCreateWindowSurface(instance, window, nullptr, &surface);
}

bool checkDeviceFeature(VkPhysicalDevice physicalDevice, const VkPhysicalDeviceFeatures& requeiredFeature) {
	VkPhysicalDeviceFeatures supportedFeatures;
	vkGetPhysicalDeviceFeatures(physicalDevice, &supportedFeatures);
	for (size_t i = 0; i < sizeof(VkPhysicalDeviceFeatures) / sizeof(VkBool32); i++) {
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

bool QVkApp::pickPhysicalDeviceAndQueueFamilies(VkPhysicalDeviceFeatures requiredDeviceFeatures, const std::vector<const char*>& requiredExtensions, const std::vector<const char*>& requiredLayers, VkSurfaceKHR surface, VkPhysicalDevice* pPhysicalDeviceOut, QVkRenderQueueFamilyIndexList* pRenderQueueFamilyIndexListOut) {
	uint32_t physicalDeviceCount;
	VkResult result = vkEnumeratePhysicalDevices(this->instance, &physicalDeviceCount, nullptr);
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

	for (auto physicalDevice : physicalDevices) {
		if (!checkDeviceFeature(physicalDevice, requiredDeviceFeatures))
			continue;

		if (!checkDeviceExtension(physicalDevice, requiredExtensions))
			continue;

		if (!checkDeviceLayer(physicalDevice, requiredLayers))
			continue;

		std::vector<VkQueueFamilyProperties> vQueueFamilyProperties;
		uint32_t uQueueFamilyPropertiesCount;
		vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &uQueueFamilyPropertiesCount, nullptr);
		vQueueFamilyProperties.resize(uQueueFamilyPropertiesCount);
		vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &uQueueFamilyPropertiesCount, vQueueFamilyProperties.data());
		int i = 0;
		auto queueFamilyPropsIter = vQueueFamilyProperties.begin();
		for (auto queueFamilyPropsIter = vQueueFamilyProperties.begin(); queueFamilyPropsIter != vQueueFamilyProperties.end(); ++queueFamilyPropsIter) {
			if ((queueFamilyPropsIter->queueFlags & VK_QUEUE_GRAPHICS_BIT)) {
				pRenderQueueFamilyIndexListOut->graphicsQueueFamily = i;
				break;
			}
			i++;
		}
		if (i == vQueueFamilyProperties.size()) {
			continue;
		}

		for (i = 0; i < vQueueFamilyProperties.size(); i++) {
			VkBool32 presentSupport = false;
			vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, surface, &presentSupport);
			if (presentSupport) {
				pRenderQueueFamilyIndexListOut->presentQueueFamily = i;
				break;
			}
		}
		if (i == vQueueFamilyProperties.size()) {
			continue;
		}

		*pPhysicalDeviceOut = physicalDevice;

		i = 0;
		for (auto queueFamilyPropsIter = vQueueFamilyProperties.begin(); queueFamilyPropsIter != vQueueFamilyProperties.end(); ++queueFamilyPropsIter) {
			if ((queueFamilyPropsIter->queueFlags & VK_QUEUE_TRANSFER_BIT)) {
				pRenderQueueFamilyIndexListOut->transferQueueFamily = i;
				break;
			}
			i++;
		}

		i = 0;
		for (auto queueFamilyPropsIter = vQueueFamilyProperties.begin(); queueFamilyPropsIter != vQueueFamilyProperties.end(); ++queueFamilyPropsIter) {
			if ((queueFamilyPropsIter->queueFlags & VK_QUEUE_COMPUTE_BIT)) {
				pRenderQueueFamilyIndexListOut->computeQueueFamily = i;
				break;
			}
			i++;
		}

	}
	if (!pRenderQueueFamilyIndexListOut->graphicsQueueFamily.has_value()) {
		return false;
	}
	std::cerr << "Graphics " << pRenderQueueFamilyIndexListOut->graphicsQueueFamily.value() << std::endl
		<< "Compute " << pRenderQueueFamilyIndexListOut->computeQueueFamily.value() << std::endl
		<< "Transfer " << pRenderQueueFamilyIndexListOut->transferQueueFamily.value() << std::endl
		<< "Present " << pRenderQueueFamilyIndexListOut->presentQueueFamily.value() << std::endl;
	return true;
}

VkResult QVkApp::initVulkan() {
	VkResult result = VK_SUCCESS;
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

	createSurface();

	VkPhysicalDeviceFeatures requiredFeatures = {};
	requiredFeatures.geometryShader = VK_TRUE;
	const std::vector<const char*> requiredExtensions = {
	VK_KHR_SWAPCHAIN_EXTENSION_NAME
	};
	const std::vector<const char*> requiredLayers = enableValidationLayers ? validationLayers : std::vector<const char*>();
	VkPhysicalDevice physicalDevice;
	QVkRenderQueueFamilyIndexList renderQueueFamilyIndexList;
	pickPhysicalDeviceAndQueueFamilies(requiredFeatures, requiredExtensions, requiredLayers, surface, &physicalDevice, &renderQueueFamilyIndexList);

	std::vector<uint32_t> requiredQueueFamilyList;
	requiredQueueFamilyList.resize(renderQueueFamilyIndexList.getQueueCount());
	renderQueueFamilyIndexList.convertQueueFamilyIndexArray(requiredQueueFamilyList.data());

	std::vector<QVkDeviceQueue*> queueOut;
	this->device.createDevice(instance, physicalDevice, requiredFeatures, requiredExtensions, requiredLayers, requiredQueueFamilyList, queueOut);

	renderQueueList.pack(&renderQueueFamilyIndexList, queueOut.data());

	return result;
}

void QVkApp::cleanupVulkan() {
	this->device.destroyDevice();
	vkDestroySurfaceKHR(instance, surface, nullptr);
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
