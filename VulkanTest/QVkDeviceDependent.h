#pragma once
#include <vulkan/vulkan.h>
#include <string>
namespace QVk {
	class QVkDevice;
	class QVkDeviceDependent
	{
	private:
		bool destroyed = false;
		friend class QVkDevice;
	protected:
		QVkDevice* pDevice;
		QVkDeviceDependent(QVkDevice* pDevice);
		virtual void destroyVkResource() = 0;
	public:
		void destroy();
		virtual std::string getTypeName() = 0;
		~QVkDeviceDependent();
	};
}