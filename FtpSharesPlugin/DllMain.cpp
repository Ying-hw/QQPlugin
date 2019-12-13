#include "stdafx.h"
#include "ftpShares.h"
#include "ftpsharesplugin_global.h"

extern "C" FTPSHARESPLUGIN_EXPORT QWidget* Handle() {
	return new ftpShares();
}