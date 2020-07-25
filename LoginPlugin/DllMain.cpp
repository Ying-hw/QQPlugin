#include "stdafx1.h"
#include "MainWidget.h"
#include "LoginSystem.h"


extern "C" LOGINPLUGIN_EXPORT AbstractWidget* Handle() {
	return new LoginSystem();
}
