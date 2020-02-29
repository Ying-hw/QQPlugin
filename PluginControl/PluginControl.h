#ifndef PLUGINCONTROL_H
#define PLUGINCONTROL_H

#include "plugincontrol_global.h"
#include "PluginInfo.h"
#include "MacroDefine.h"

class PLUGINCONTROL_EXPORT PluginControl
{
public:
	PluginControl();
	~PluginControl();
public:
	static std::vector<PluginInfo> m_VecPlugins;
private:
	void SendPluginConfig();
};

#endif // PLUGINCONTROL_H
