#include "QVkApp.h"
#include <iostream>

int main(int argc, const char** arv) {
	QVk::QVkApp app;
	try {
		app.init();
		app.mainLoop();
		app.destroy();
	}
	catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
		return 1;
	}
	return 0;
}