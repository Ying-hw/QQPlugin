#ifndef FRIENDLIST_H
#define FRIENDLIST_H

#include "ui_FriendList.h"
#include "MacroDefine.h"
#include "friendlistplugin_global.h"
#include "Animation.h"
class Animation;
class FRIENDLISTPLUGIN_EXPORT FriendList : public Animation
{
	Q_OBJECT

public:
	FriendList(QWidget *parent = 0);
	~FriendList();

private:
	Ui::FriendList ui;
};

#endif // FRIENDLIST_H
