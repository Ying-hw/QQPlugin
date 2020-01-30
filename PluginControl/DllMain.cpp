#include "stdafx.h"
#include "PluginControl.h"
#include "plugincontrol_global.h"

extern "C" PLUGINCONTROL_EXPORT void Handle() {
	PluginControl* plugin = new PluginControl();
}