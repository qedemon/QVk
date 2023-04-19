#include "QVkBuffer.h"
#include "QVkDevice.h"

using namespace QVk;

QVkBuffer::QVkBuffer(QVkDevice* pDevice, VkDeviceSize size, VkBufferUsageFlags usage) : QVkBackedByMemory(pDevice) {
	bufferSize = size;
	this->usage = usage;
	VkBufferCreateInfo bufferCreateInfo = {};
	bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	bufferCreateInfo.usage = usage;
	bufferCreateInfo.size = size;
	this->vkBuffer = VK_NULL_HANDLE;
	VkResult result = vkCreateBuffer(this->pDevice->getLogicalDevice(), &bufferCreateInfo, nullptr, &this->vkBuffer);
	if (result != VK_SUCCESS) {
		throw std::exception("QVkBuffer constructor error : cannot create VkBuffer object");
	}
}
bool QVkBuffer::_bindVkMemory(QVkMemoryManager* pBackMemory, VkDeviceSize offset) {
	return false;
}

void QVkBuffer::getMemoryRequirement(VkMemoryRequirements* pMemoryRequirement) {
	vkGetBufferMemoryRequirements(pDevice->getLogicalDevice(), vkBuffer, pMemoryRequirement);
}