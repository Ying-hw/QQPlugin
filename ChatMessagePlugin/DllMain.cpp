#include "stdafx.h"
#include "ChatMessage.h"
#include "ChatMessage_global.h"

extern "C" CHATMESSAGEPLUGIN_EXPORT QWidget* Handle() {
	return new ChatMessage();
}