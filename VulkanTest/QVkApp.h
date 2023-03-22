#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vector>
#include "QVkDevice.h"

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
		std::vector<VkPhysicalDevice> physicalDevices;
		std::vector<VkPhysicalDeviceProperties> physicalDeviceProperties;
		
		QVkDevice device;

	private:
		bool initWindow();
		VkResult initVulkan();
		bool checkValidationLayerSupport();
		VkResult createInstance();
		void cleanupVulkan();
	public:
		QVkApp();
		void init();
		void mainLoop();
		void destroy();
	};
}