#include "stdafx1.h"
#include "MainWidget.h"
#include "LoginSystem.h"


extern "C" LOGINPLUGIN_EXPORT QWidget* Handle() {
	return new LoginSystem();
}