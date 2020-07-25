#ifndef __APPLYJOINFRIEND__
#define __APPLYJOINFRIEND__

#include "AbstractWidget.h"
#include "ui_ApplyJoinFriend.h"

class ApplyJoinFriend : public AbstractWidget
{
	Q_OBJECT

public:
	ApplyJoinFriend(QString strName, QString strAttr, QPixmap& arrayImage, bool isFriend, AbstractWidget *parent = Q_NULLPTR);
	~ApplyJoinFriend();

private slots:
	void SlotBtnSend();
private:
	Ui::ApplyJoinFriend ui;
	bool m_IsFriend;
};

#endif
