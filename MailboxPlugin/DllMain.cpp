#include "stdafx.h"
#include "Mailbox.h"
#include "mailboxplugin_global.h"

extern "C" MAILBOXPLUGIN_EXPORT QWidget* Handle() {
	return new Mailbox();
}