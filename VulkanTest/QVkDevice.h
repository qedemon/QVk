#pragma once
#include <vulkan/vulkan.h>
#include "QVkDeviceDependent.h"
#include "QVkDeviceDependentList.h"
#include <vector>
#include <set>
#include <optional>

namespace QVk {
	class QVkDevice {
	private:
		VkPhysicalDeviceFeatures deviceFeatures;
		std::vector<const char*> deviceExtensions;
		std::vector<const char*> deviceLayers;
		std::set<QVkDeviceDependent*> deviceDependents;
		QVkMemoryPool* pMemoryPool=nullptr;

		VkInstance instance=VK_NULL_HANDLE;
		VkDevice device=VK_NULL_HANDLE;
		VkPhysicalDevice physicalDevice=VK_NULL_HANDLE;

		void registerDeviceDependent(QVkDeviceDependent* pDependent);
		void unregisterDeviceDependent(QVkDeviceDependent* pDependent);
		friend class QVkDeviceDependent;
	public:
		QVkDevice();
		//bool setupPhysicalDevice(VkInstance instance, VkPhysicalDeviceFeatures requiredDeviceFeatures, const std::vector<const char*>& requiredExtensions, const std::vector<const char*>& requiredLayers, VkSurfaceKHR surface=VK_NULL_HANDLE);
		VkResult createDevice(VkInstance instance, VkPhysicalDevice, VkPhysicalDeviceFeatures requiredDeviceFeatures, const std::vector<const char*>& requiredExtensions, const std::vector<const char*>& requiredLayers, const std::vector<uint32_t>& queueFamilyIndices, std::vector<QVkDeviceQueue*>& deviceQueues);
		void destroyDevice();
		inline VkPhysicalDevice getPhysicalDevice() { return this->physicalDevice; }
		inline VkDevice getLogicalDevice() { return this->device; }
	public:
		QVkMemoryManager* createMemory(uint32_t memoryIndex);
		QVkMemoryPool* createMemoryPool();
		QVkBuffer* createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, bool mappable);
	};
}