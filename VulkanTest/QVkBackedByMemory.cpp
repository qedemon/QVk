#include "QVkBackedByMemory.h"

using namespace QVk;

QVkBackedByMemory::QVkBackedByMemory(QVkDevice* pDevice) : QVkDeviceDependent(pDevice) {
	pBackMemory = nullptr;
	mappable = false;
}


bool QVkBackedByMemory::bindMemory(QVkMemoryManager* pBackMemory, VkDeviceSize offset, VkDeviceSize size, bool mappable) {
	this->pBackMemory = pBackMemory;
	this->offset = offset;
	this->size = size;
	this->mappable = mappable;
	return this->_bindVkMemory(pBackMemory, offset);
}

void* QVkBackedByMemory::map(VkDeviceSize offset, VkDeviceSize Size) {
	if (!this->mappable) {
		throw std::exception("QVkBackedMemory::map error : this is not mappable");
	}
	return this->pBackMemory->mapMemory(offset+this->offset.value(), this->size.value());
}

void QVkBackedByMemory::unmap() {
	this->pBackMemory->unmapMemory();
}

VkDeviceMemory QVkBackedByMemory::getVkDeviceMemory() {
	return this->pBackMemory->getVkDeviceMemory();
}