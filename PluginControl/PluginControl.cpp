#include "stdafx.h"
#include "PluginControl.h"


std::vector<PluginInfo> PluginControl::m_VecPlugins;
PluginControl::PluginControl()
{
	SendPluginConfig();
}

PluginControl::~PluginControl() 
{

}

void PluginControl::SendPluginConfig()
{
	m_VecPlugins.emplace_back("ChatMessagePlugin", ".", false);
	m_VecPlugins.emplace_back("FriendListPlugin", ".", false);
	m_VecPlugins.emplace_back("FtpSharesPlugin", ".", false);
	m_VecPlugins.emplace_back("LoginPlugin", ".", true);
	m_VecPlugins.emplace_back("MailboxPlugin", ".", false);
	m_VecPlugins.emplace_back("QQSpacePlugin", ".", false);
	m_VecPlugins.emplace_back("VideoChatPlugin", ".", false);
	
	SEND_SIGNAL(Signal_::MAKEPLUGINFILE, &m_VecPlugins); 
}
