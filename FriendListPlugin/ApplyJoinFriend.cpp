#include "stdafx.h"
#include "ApplyJoinFriend.h"
#include "NetProtocConfig.pb.h"
#include "FriendList.h"
#include "ProsessMessage.h"
#include "AddFriend.h"

ApplyJoinFriend::ApplyJoinFriend(TargetInfor& infor, AbstractWidget *parent /*= Q_NULLPTR*/) : AbstractWidget(parent), m_IsFriend(infor.m_IsFriend)
{
	ui.setupUi(this);
	ui.LabImage->setPixmap(infor.pix);
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
	AddInformation add_info;
	add_info.set_type((AddInformation_TargetType)(int)m_IsFriend);
	add_info.set_reason(ui.EditContent->toPlainText().toStdString());
	add_info.set_targetaccount(ui.LabNumber->text().toStdString());
	add_info.set_fromaccount(FriendList::m_pUserNumber->toStdString());
	QHostAddress host("192.168.1.17");
	ProsessMessage* m_ProMsg = new ProsessMessage(AbstractNetWork::ProtoType::TCP, host, 7007, this);
	SendSIG(Signal_::INITIALIZENETWORK, m_ProMsg);
	protocol proto;
	proto.set_allocated_addinfor(&add_info);
	proto.set_type(protocol_MsgType_tcp);
	m_ProMsg->SendMsg(QString::fromStdString(proto.SerializeAsString()));
}
