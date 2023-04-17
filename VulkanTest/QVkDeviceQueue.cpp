#include "QVkDeviceQueue.h"

using namespace QVk;

QVkDeviceQueue::QVkDeviceQueue(QVkDevice* pDevice, VkQueue deviceQueue, uint32_t queueIndex) :
	QVkDeviceDependent(pDevice), deviceQueue(deviceQueue), queueIndex(queueIndex) {
}

void QVkDeviceQueue::destroyVkResource() {

}

std::string QVkDeviceQueue::getTypeName() {
	return "Queue";
}