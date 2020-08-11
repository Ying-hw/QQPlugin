#include "stdafx.h"
#include "ApplyJoinFriend.h"
#include "NetProtocConfig.pb.h"
#include "FriendList.h"
#include "ProsessMessage.h"
#include "AddFriend.h"

ApplyJoinFriend::ApplyJoinFriend(TargetInfor& infor, AbstractWidget *parent /*= Q_NULLPTR*/) : AbstractWidget(parent), m_IsFriend(infor.m_IsFriend)
{
	ui.setupUi(this);
	ui.LabImage->setPixmap(FriendList::PixmapToRound(infor.pix, 80));
	ui.LabName->setText(infor.m_strName);
	ui.LabNumber->setText(infor.m_strNumber);
	if (m_IsFriend) {
		ui.LabGender->setText(ui.LabGender->text() + infor.m_Gender);
		ui.LabAge->setText(ui.LabAge->text() + QString::number(infor.m_Age));
		ui.LabOccupational->setText(ui.LabOccupational->text() + infor.m_Occupational);
		ui.LabSchool->setText(ui.LabSchool->text() + infor.m_strSchool);
		ui.LabBirthPlace->setText(ui.LabBirthPlace->text() + infor.m_strBirthPlace);
		ui.LabAddress->setText(ui.LabAddress->text() + infor.m_Address);
		ui.LabGroupType->hide();
		ui.LabCount->hide();
	}
	else {
		ui.LabGender->hide();
		ui.LabAge->hide();
		ui.LabOccupational->hide();
		ui.LabSchool->hide();
		ui.LabBirthPlace->hide();
		ui.LabAddress->hide();
		ui.LabCount->setText(ui.LabCount->text() + QString::number(infor.memberCount));
		ui.LabGroupType->setText(ui.LabGroupType->text() + infor.m_strGroupType);
	}
	connect(ui.BtnSend, SIGNAL(clicked()), this, SLOT(SlotBtnSend()));
}

ApplyJoinFriend::~ApplyJoinFriend()
{
}

void ApplyJoinFriend::SlotBtnSend()
{
	protocol proto;
	proto.mutable_addinfor()->set_type((AddInformation_TargetType)(int)m_IsFriend);
	proto.mutable_addinfor()->set_reason(ui.EditContent->toPlainText().toStdString());
	proto.mutable_addinfor()->set_targetaccount(ui.LabNumber->text().toStdString());
	proto.mutable_addinfor()->set_fromaccount(FriendList::m_pUserNumber->toStdString());
	
	proto.set_type(protocol_MsgType_tcp);
	FriendList::m_NetWorkProsess->SendMsg(QString::fromStdString(proto.SerializeAsString()));
}
