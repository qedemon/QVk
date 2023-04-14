#pragma once
#include <vulkan/vulkan.h>
#include <vector>
#include <optional>

namespace QVk {
	typedef struct QVkQueueFamilies {
		std::optional<uint32_t> graphicsQueueFamilyIndex;
		std::optional<uint32_t> computeQueueFamiliyIndex;
		std::optional<uint32_t> transferQueueFamiliyIndex;
	}QVkQueueFamilies;

	class QVkDevice {
	private:
		VkPhysicalDeviceFeatures deviceFeatures;
		std::vector<const char*> deviceExtensions;
		std::vector<const char*> deviceLayers;
		
		VkDevice device;
		VkPhysicalDevice physicalDevice;
		QVkQueueFamilies selectedQueueFamilies;

		VkQueue graphicsQueue;
		VkQueue computeQueue;
		VkQueue transferQueue;
	public:
		QVkDevice();
		bool setupPhysicalDevice(std::vector<VkPhysicalDevice> &phyDevs, std::vector<VkPhysicalDeviceProperties> &phyDevProps, VkPhysicalDeviceFeatures requiredDeviceFeatures, const std::vector<const char*>& requiredExtensions, const std::vector<const char*>& requiredLayers);
		VkResult createDevice();
		void destroyDevice();
		inline VkPhysicalDevice getPhysicalDevice() { return this->physicalDevice; }
		inline VkDevice getLogicalDevice() { return this->device; }
	};
}