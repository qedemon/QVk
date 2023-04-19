#pragma once
#include "QVkBackedByMemory.h"
namespace QVk {
    class QVkBuffer :
        public QVkBackedByMemory
    {
    private:
        VkBuffer vkBuffer;
        VkDeviceSize bufferSize;
        VkBufferUsageFlags usage;
    private:
        QVkBuffer(QVkDevice* pDevice, VkDeviceSize size, VkBufferUsageFlags usage);
        virtual bool _bindVkMemory(QVkMemoryManager* pBackMemory, VkDeviceSize offset);
    public:
        virtual void getMemoryRequirement(VkMemoryRequirements* pMemoryRequirement);
    };
}

