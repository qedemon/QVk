#include "QVkBackedByMemory.h"

using namespace QVk;

QVkBackedByMemory::QVkBackedByMemory(QVkDevice* pDevice, QVkMemory* pBackMemory, VkDeviceSize offset, VkDeviceSize size, bool mappable) :
	QVkDeviceDependent(pDevice), pBackMemory(pBackMemory), offset(offset), size(size), mappable(mappable) {

}

void* QVkBackedByMemory::map() {
	if (!this->mappable) {
		throw std::exception("QVkBackedMemory::map error : this is not mappable");
	}
	return this->pBackMemory->mapMemory(offset, size);
}

void QVkBackedByMemory::unmap() {
	return this->pBackMemory->unmapMemory();
}