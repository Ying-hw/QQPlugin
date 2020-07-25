#include "stdafx.h"
#include "ApplyJoinFriend.h"
#include "NetProtocConfig.pb.h"
#include "FriendList.h"
#include "ProsessMessage.h"

ApplyJoinFriend::ApplyJoinFriend(QString strName, QString strAttr, QPixmap& arrayImage, bool isFriend, AbstractWidget *parent /*= Q_NULLPTR*/) : AbstractWidget(parent), m_IsFriend(isFriend)
{
	ui.setupUi(this);
	ui.LabImage->setPixmap(arrayImage);
	ui.LabName->setText(strName);
	ui.LabNumber->setText(strAttr.section(" ",0,0));
	ui.LabAge->setText(strAttr.section(" ", 1, 1));
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
	proto.set_type(protocol_Type_tcp);
	m_ProMsg->SendMsg(QString::fromStdString(proto.SerializeAsString()));
}
