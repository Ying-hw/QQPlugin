#include "stdafx.h"
#include "ChatMessage.h"
#include "ChatMessage_global.h"

extern "C" CHATMESSAGEPLUGIN_EXPORT AbstractWidget* Handle() {
	return new ChatMessage();
}