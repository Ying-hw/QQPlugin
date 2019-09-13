#include "stdafx1.h"
#include "MainWidget.h"
#include "Login.h"

extern "C" LOGINPLUGIN_EXPORT MainWidget* Handle(void *data) {
	return new Login();
}