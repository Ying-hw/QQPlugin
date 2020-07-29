#ifndef __APPLYJOINFRIEND__
#define __APPLYJOINFRIEND__

#include "AbstractWidget.h"
#include "ui_ApplyJoinFriend.h"
struct TargetInfor;

class ApplyJoinFriend : public AbstractWidget
{
	Q_OBJECT

public:
	ApplyJoinFriend(TargetInfor& infor, AbstractWidget *parent = Q_NULLPTR);
	~ApplyJoinFriend();

private slots:
	void SlotBtnSend();
private:
	Ui::ApplyJoinFriend ui;
	bool m_IsFriend;
};

#endif
