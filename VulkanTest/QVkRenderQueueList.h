#pragma once
#include <vulkan/vulkan.h>
#include "QVkDeviceQueue.h"
#include <optional>

namespace QVk {
	typedef struct QVkRenderQueueFamilyIndexList {
		std::optional<uint32_t> graphicsQueueFamily;
		std::optional<uint32_t> computeQueueFamily;
		std::optional<uint32_t> transferQueueFamily;
		std::optional<uint32_t> presentQueueFamily;
		uint32_t getQueueCount() {
			uint32_t queueCount = 0;
			if (graphicsQueueFamily.has_value())
				queueCount++;
			if (computeQueueFamily.has_value())
				queueCount++;
			if (transferQueueFamily.has_value())
				queueCount++;
			if (presentQueueFamily.has_value())
				queueCount++;
			return queueCount;
		}
		void convertQueueFamilyIndexArray(uint32_t* pOut) {
			if (pOut != nullptr) {
				uint32_t queueCount = 0;
				if (graphicsQueueFamily.has_value()) {
					pOut[queueCount] = graphicsQueueFamily.value();
					queueCount++;
				}
				if (computeQueueFamily.has_value()) {
					pOut[queueCount] = computeQueueFamily.value();
					queueCount++;
				}
				if (transferQueueFamily.has_value()) {
					pOut[queueCount] = transferQueueFamily.value();
					queueCount++;
				}
				if (presentQueueFamily.has_value()) {
					pOut[queueCount] = transferQueueFamily.value();
					queueCount++;
				}
				return;
			}
		}
	}QVkRenderQueueFamilyIndexList;
	typedef struct QVkRenderQueueList {
		std::optional<QVkDeviceQueue*> graphicsQueue;
		std::optional<QVkDeviceQueue*> computeQueue;
		std::optional<QVkDeviceQueue*> transferQueue;
		std::optional<QVkDeviceQueue*> presentQueue;
		void pack(const QVkRenderQueueFamilyIndexList* pRenderQueueFamilyIndexList, QVkDeviceQueue** ppQueues) {
			size_t i = 0;
			if (pRenderQueueFamilyIndexList->graphicsQueueFamily.has_value()) {
				graphicsQueue = ppQueues[i];
				i++;
			}
			if (pRenderQueueFamilyIndexList->computeQueueFamily.has_value()) {
				computeQueue = ppQueues[i];
				i++;
			}
			if (pRenderQueueFamilyIndexList->transferQueueFamily.has_value()) {
				transferQueue = ppQueues[i];
				i++;
			}
			if (pRenderQueueFamilyIndexList->presentQueueFamily.has_value()) {
				presentQueue = ppQueues[i];
				i++;
			}
		}
	}QVkRenderQueueList;
}