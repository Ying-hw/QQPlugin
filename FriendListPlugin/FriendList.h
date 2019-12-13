#ifndef FRIENDLIST_H
#define FRIENDLIST_H

#include <QWidget>
#include "ui_FriendList.h"
#include "friendlistplugin_global.h"

class FRIENDLISTPLUGIN_EXPORT FriendList : public QWidget
{
	Q_OBJECT

public:
	FriendList(QWidget *parent = 0);
	~FriendList();

private:
	Ui::FriendList ui;
};

#endif // FRIENDLIST_H
