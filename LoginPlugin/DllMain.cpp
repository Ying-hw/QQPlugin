#include "stdafx1.h"
#include "MainWidget.h"
#include "Login.h"

extern "C" LOGINPLUGIN_EXPORT void Handle() {
	Login* login = new Login();
}