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
		VkDevice device;
		VkPhysicalDevice physicalDevice;
		QVkQueueFamilies selectedQueueFamilies;

		VkQueue graphicsQueue;
	public:
		QVkDevice();
		bool setupPhysicalDevice(std::vector<VkPhysicalDevice> &phyDevs, std::vector<VkPhysicalDeviceProperties> &phyDevProps);
		VkResult createDevice(VkPhysicalDeviceFeatures deviceFeatures, const std::vector<const char*>& requiredExtensions, const std::vector<const char*>& requiredLayers);
		void destroyDevice();
	};
}