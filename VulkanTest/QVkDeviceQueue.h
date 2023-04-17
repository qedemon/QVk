#pragma once
#include <vulkan/vulkan.h>
#include "QVkDeviceDependent.h"
#include "QVkDevice.h"
namespace QVk {
    class QVkDeviceQueue :
        public QVkDeviceDependent
    {
    private:
        VkQueue deviceQueue;
        uint32_t queueIndex;
    private:
        void destroyVkResource();
        QVkDeviceQueue(QVkDevice* pDevice, VkQueue deviceQueue, uint32_t queueIndex);
        friend class QVkDevice;
    public:
        std::string getTypeName();
    };
}

