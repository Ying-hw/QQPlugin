#include "stdafx.h"
#include "FriendList.h"
#include "friendlistplugin_global.h"

extern "C" FRIENDLISTPLUGIN_EXPORT QWidget* Handle() {
	return new FriendList();
}