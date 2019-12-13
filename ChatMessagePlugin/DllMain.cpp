#include "stdafx.h"
#include "ChatMessage.h"
#include "ChatMessage_global.h"

extern "C" FRIENDLISTPLUGIN_EXPORT QWidget* Handle() {
	return new ChatMessage();
}