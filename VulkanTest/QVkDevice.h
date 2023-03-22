#pragma once
#include <vulkan/vulkan.h>
#include <vector>

namespace QVk {
	typedef struct QVkQueueFamilies {
		uint32_t graphicsQueueFamilyIndex;
		uint32_t computeQueueFamiliyIndex;
		uint32_t transferQueueFamiliyIndex;
	}QVkQueueFamilies;

	class QVkDevice {
	private:
		VkDevice device;
		VkPhysicalDevice physicalDevice;
		QVkQueueFamilies selectedQueueFamilies;
	public:
		QVkDevice();
		bool setupPhysicalDevice(std::vector<VkPhysicalDevice> &phyDevs, std::vector<VkPhysicalDeviceProperties> &phyDevProps);
	};
}