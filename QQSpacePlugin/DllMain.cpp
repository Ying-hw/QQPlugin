#include "stdafx.h"
#include "QQSpace.h"
#include "qqspaceplugin_global.h"

extern "C" QQSPACEPLUGIN_EXPORT QWidget* Handle() {
	return new QQSpace();
}