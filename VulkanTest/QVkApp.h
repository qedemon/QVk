#include <vulkan/vulkan.h>
#include <vector>
#include "QVkDevice.h"

namespace QVk {

	class QVkApp {
	private:
		VkInstance instance;
		std::vector<VkPhysicalDevice> physicalDevices;
		std::vector<VkPhysicalDeviceProperties> physicalDeviceProperties;
		
		QVkDevice device;

	private:
		VkResult createInstance();
	public:
		QVkApp();
		VkResult init();
		void destroy();
	};
}