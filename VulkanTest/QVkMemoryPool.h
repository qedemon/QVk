#pragma once
#include "QVkMemoryManager.h"
namespace QVk {
	class QVkMemoryPool :QVkDeviceDependent
	{
	private:
		std::vector<QVkMemoryManager*> memoryManagers;
	protected:
		void destroyVkResource();
		QVkMemoryPool(QVkDevice* pDevice);
	public:
		std::string getTypeName();
		friend class QVkDevice;
	};
}

