#include "QVkDeviceDependent.h"
#include "QVkDevice.h"

using namespace QVk;

QVkDeviceDependent::QVkDeviceDependent(QVkDevice* pDevice) {
	this->pDevice = pDevice;
	this->pDevice->registerDeviceDependent(this);
}

QVkDeviceDependent::~QVkDeviceDependent() {
	this->destroy();
}

void QVkDeviceDependent::destroy() {
	if (!destroyed) {
		this->destroyVkResource();
		this->pDevice->unregisterDeviceDependent(this);
		destroyed = true;
	}
}