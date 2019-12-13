#include "stdafx.h"
#include "VideoChat.h"
#include "videochatplugin_global.h"

extern "C" VIDEOCHATPLUGIN_EXPORT QWidget* Handle() {
	return new VideoChat();
}