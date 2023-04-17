#pragma once
#define VK_USE_PLATFORM_WIN32_KHR
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>
#include <vector>
#include "QVkDevice.h"
#include "QVkRenderQueueList.h"
#include "QVkMemoryManager.h"

namespace QVk {

	class QVkApp {
	private:
		GLFWwindow* window;
		const uint32_t WIDTH = 800;
		const uint32_t HEIGHT = 600;
	
		const std::vector<const char*> validationLayers = {
		"VK_LAYER_KHRONOS_validation"
		};
#ifdef NDEBUG
		const bool enableValidationLayers = false;
#else
		const bool enableValidationLayers = true;
#endif

		VkInstance instance;
		VkDebugUtilsMessengerEXT debugMessenger;
		std::vector<VkPhysicalDevice> physicalDevices;
		std::vector<VkPhysicalDeviceProperties> physicalDeviceProperties;
		
		QVkDevice device;
		VkSurfaceKHR surface;
		QVkRenderQueueList renderQueueList;

	private:
		bool initWindow();
		VkResult initVulkan();
		bool checkValidationLayerSupport();
		static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData);
		std::vector<const char*> getRequiredExtensions();
		VkResult createInstance();
		void setupDebugMessenger();
		void createSurface();
		bool pickPhysicalDeviceAndQueueFamilies(VkPhysicalDeviceFeatures requiredDeviceFeatures, const std::vector<const char*>& requiredExtensions, const std::vector<const char*>& requiredLayers, VkSurfaceKHR surface, VkPhysicalDevice* pPhysicalDeviceOut, QVkRenderQueueFamilyIndexList* pRenderQueueFamilyIndexListOut);
		void cleanupVulkan();
	public:
		QVkApp();
		void init();
		void mainLoop();
		void destroy();
	};
}