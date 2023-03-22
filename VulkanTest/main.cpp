#include "QVkApp.h"

int main(int argc, const char** arv) {
	QVk::QVkApp app;
	app.init();
	app.mainLoop();
	app.destroy();
	return 0;
}