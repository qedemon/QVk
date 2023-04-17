#include "QVkMemoryPool.h"

using namespace QVk;

QVkMemoryPool::QVkMemoryPool(QVkDevice* pDevice) : QVkDeviceDependent(pDevice) {

}

void QVkMemoryPool::destroyVkResource() {
	for (auto manager : memoryManagers) {
		manager->destroy();
	}
	memoryManagers.clear();
}

std::string QVkMemoryPool::getTypeName() {
	return "Memory Pool";
}