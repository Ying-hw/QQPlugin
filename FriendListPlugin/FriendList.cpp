#include "stdafx.h"
#include "MacroDefine.h"
#include "FriendList.h"

FriendList::FriendList(QWidget *parent)
	: Animation(parent)
{
	ui.setupUi(this);
	ui.listV_Message->hide();
}

FriendList::~FriendList()
{

}
