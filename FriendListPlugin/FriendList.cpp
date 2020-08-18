#include "stdafx.h"
#include "MessageTemplate.h"
#include "FriendList.h"
#include "AddFriend.h"

FriendList* g_FriendList = NULL;
QString* FriendList::m_pUserNumber = NULL;
#define    CONDIGFILE   "../Data/Image/Avatar.jpg"

ProsessMessage* FriendList::m_NetWorkProsess = NULL;
FriendList::FriendList(QWidget *parent) : AbstractWidget(parent), m_pSystemMenu(NULL)
{
	ui.setupUi(this);
	g_FriendList = this;
	m_pFriendTree = new QTreeWidgetItem(ui.Friend_List);
	m_pGroupTree = new QTreeWidgetItem(ui.Friend_List);
	m_pGroupTree->setText(0, QString::fromLocal8Bit("群组列表"));
	m_pFriendTree->setText(0, QString::fromLocal8Bit("好友列表"));
	const QString* strUserName = (QString *)GET_MESSAGE(0);
	m_pUserNumber = (QString *)GET_MESSAGE(1);
	ui.labUserName->setText(*strUserName);
	delete strUserName;
	strUserName = nullptr; 
	m_NetWorkProsess = new ProsessMessage(AbstractNetWork::ProtoType::TCP, "33a15e2655.qicp.vip", 54813, g_FriendList);
	SendSIG(Signal_::INITIALIZENETWORK, m_NetWorkProsess, Signal_Type::CMD);
	InitFriendList();
	InitGroupList();
	InitQQSpaceList(); 
	RecoveryChatRecord();
	ui.BtnAdd->setContextMenuPolicy(Qt::CustomContextMenu);
	QAction* pAction_enter = new QAction(this);
	QAction* pAction_No_Enter = new QAction(this);
	pAction_enter->setIcon(QIcon("../Data/Image/User.png"));
	pAction_No_Enter->setIcon(QIcon("../Data/Image/User_group1.png"));
	pAction_enter->setIconText(QString::fromLocal8Bit("添加好友"));
	pAction_No_Enter->setIconText(QString::fromLocal8Bit("添加群"));
	ui.BtnMenu->setContextMenuPolicy(Qt::CustomContextMenu);
	QAction* acTionImage = new QAction(QString::fromLocal8Bit("更换头像"), this);
	QAction* About = new QAction(QString::fromLocal8Bit("关于"), this);
	QAction* EditInfor = new QAction(QString::fromLocal8Bit("编辑资料"), this);
	m_pSystemMenu = new QMenu(this);
	m_pSystemMenu->addAction(About);
	m_pSystemMenu->addAction(acTionImage);
	m_pSystemMenu->addAction(EditInfor);
	ui.BtnMenu->setMenu(m_pSystemMenu);
	connect(pAction_enter, SIGNAL(triggered(bool)), this, SLOT(SlotAdd(bool)));
	connect(pAction_No_Enter, SIGNAL(triggered(bool)), this, SLOT(SlotAdd(bool)));
	connect(ui.BtnFriend, SIGNAL(clicked()), this, SLOT(SwitchFriMsgSpace()));
	connect(ui.BtnSpace, SIGNAL(clicked()), this, SLOT(SwitchFriMsgSpace()));
	connect(ui.BtnMessage, SIGNAL(clicked()), this, SLOT(SwitchFriMsgSpace()));
	connect(acTionImage, SIGNAL(triggered(bool)), this, SLOT(SlotAdd(bool)));
	connect(About, SIGNAL(triggered(bool)), this, SLOT(SlotAdd(bool)));
	connect(EditInfor, SIGNAL(triggered(bool)), this, SLOT(SlotAdd(bool)));
	connect(ui.ComState, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(SlotChangedState(const QString&)));
	emit ui.BtnFriend->click();
	sqlPlugin::DataStructDefine MyInfo;
	QPixmap pix;
	GET_DATA(MyInfo, QString(SELECT_USER).arg(*m_pUserNumber));
	QByteArray arrayImage = MyInfo.m_lstAllData[0]["IMAGE"].toByteArray();
	if (pix.loadFromData(arrayImage)) {
		ui.LabImage->setPixmap(PixmapToRound(pix, 40));
	}
	QMenu* pMenu = new QMenu(this);
	pMenu->addAction(pAction_enter);
	pMenu->addAction(pAction_No_Enter);
	ui.BtnAdd->setMenu(pMenu);
	SlotChangedState(ui.ComState->currentText());
}

FriendList::~FriendList()
{
	SlotChangedState(QString::fromLocal8Bit("离线"));
	if (m_NetWorkProsess) {
		delete m_NetWorkProsess;
		m_NetWorkProsess = NULL;
	}
}

void FriendList::RecoveryChatRecord()
{
	sqlPlugin::DataStructDefine dataLib, dataLib1;
	QMap<QString, protocol> mapChat;
	QString strSelectFriend = QString(SELECT_CHAT_CONTENT).arg(*m_pUserNumber);
	sqlPlugin::DataStructDefine& target = GET_DATA(dataLib, strSelectFriend);
	for (int i = 0; i < target.m_lstAllData.size();i++) {
		protocol protoContent;
		std::string array = QString::fromUtf8(target.m_lstAllData[i]["CHAT_RECORD"].toByteArray().data()).toStdString();
		if (!array.empty() && protoContent.ParseFromString(array)) {
			QString strAccount = target.m_lstAllData[i]["FRIEND_ACCOUNT"].toString();
			if (strAccount == *m_pUserNumber)
				strAccount = target.m_lstAllData[i]["USER_ACCOUNT"].toString();
			mapChat[strAccount] = protoContent;
		}
		std::string unknowArray = QString::fromUtf8(target.m_lstAllData[i]["UNKNOW_MSG"].toByteArray().data()).toStdString();
		protocol pro;
		if (!unknowArray.empty() && pro.ParseFromString(unknowArray)) {
			protoContent.MergeFrom(pro);
			QString strAccount = target.m_lstAllData[i]["FRIEND_ACCOUNT"].toString();
			if (strAccount == *m_pUserNumber)
				strAccount = target.m_lstAllData[i]["USER_ACCOUNT"].toString();
			mapChat[strAccount] = protoContent;
		}
	}
	for (QMap<QString, protocol>::const_iterator it = mapChat.constBegin();
		it != mapChat.constEnd(); it++) {
		QString strSelectUser = QString(SELECT_USER).arg(it.key());
		sqlPlugin::DataStructDefine& data = GET_DATA(dataLib1, strSelectUser);
		if (!data.m_lstAllData.isEmpty()) {
			QString strName = data.m_lstAllData[0]["USER_NAME"].toString();
			QByteArray array = data.m_lstAllData[0]["IMAGE"].toByteArray();
			SetMessage_ListUi(it.key(), strName, array, it->count());
		}
	}
}

void FriendList::SetMessage_ListUi(const QString& strNum, const QString& strName, const QByteArray& byteImage, bool isFriend)
{
	CustomToolButton* pCusBu = new CustomToolButton(ui.listV_Message);
	QPixmap pix;
	if (pix.loadFromData(byteImage)) {
		QPixmap newpix = pix.scaled(100, 100, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
		newpix = PixmapToRound(newpix, 50);
		pCusBu->setIcon(newpix);
		pCusBu->setText(strName);
		pCusBu->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
		pCusBu->setAutoRaise(true);
		isFriend ? connect(pCusBu, SIGNAL(clicked()), this, SLOT(SlotStartChatFromMessage())) : connect(pCusBu, SIGNAL(clicked()), this, SLOT(SlotStartGroupChatFromMessage()));
		m_mapMesssage[pCusBu] = strNum;
	}
	QListWidgetItem* lstitem = new QListWidgetItem(ui.listV_Message);
	lstitem->setSizeHint(QSize(ui.listV_Message->width(), 80));
	ui.listV_Message->addItem(lstitem);
	ui.listV_Message->setItemWidget(lstitem, pCusBu);
}

void FriendList::InitFriendList()
{
	sqlPlugin::DataStructDefine dataLib, dataLib1, dataLib2;
	QString strFriend = QString(SELECT_FRIEND).arg(*m_pUserNumber);
	sqlPlugin::DataStructDefine& data = GET_DATA(dataLib, strFriend);
	QPixmap pix;
	if (!data.m_lstAllData.isEmpty()) 
		for (int i = 0; i < data.m_lstAllData.size(); i++) {
			QString strTargetNum = data.m_lstAllData[i]["FRIEND_ACCOUNT"].toString();
			if (strTargetNum == *m_pUserNumber)
				strTargetNum = data.m_lstAllData[i]["USER_ACCOUNT"].toString();
			sqlPlugin::DataStructDefine& friendata = GET_DATA(dataLib1, QString(SELECT_USER).arg(strTargetNum));
			QByteArray arrayImage = friendata.m_lstAllData[0]["IMAGE"].toByteArray();
			QString strName = friendata.m_lstAllData[0]["USER_NAME"].toString();
			if (pix.loadFromData(arrayImage)) {
				QTreeWidgetItem* itemTree = new QTreeWidgetItem(m_pFriendTree);
				QPixmap newpix = pix.scaled(100, 100, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);				
				CustomToolButton* pToolBu = new CustomToolButton(this);
				sqlPlugin::DataStructDefine& userState = GET_DATA(dataLib2, QString(SELECT_STATE).arg(strTargetNum));
				if (!userState.m_lstAllData.isEmpty()) {
					QString strState = userState.m_lstAllData[0]["STATE"].toString();
					if (strState == QString::fromLocal8Bit("离线") || strState == QString::fromLocal8Bit("隐身"))
						newpix = QPixmap::fromImage(convertImage(QIcon(newpix), pToolBu->iconSize()));
				}
				newpix = PixmapToRound(newpix, 100);
				pToolBu->setIcon(newpix);
				pToolBu->setIconSize(QSize(50, 50));
				pToolBu->setText(strName);
				pToolBu->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
				pToolBu->setAutoRaise(true);
				connect(pToolBu, SIGNAL(clicked()), this, SLOT(SlotStartChat()));
				m_mapFriend[pToolBu] = friendata.m_lstAllData[0]["USER_NUMBER"].toString();
				pToolBu->setMinimumHeight(60);
				ui.Friend_List->setItemWidget(itemTree, 0, pToolBu);
			}
		}
}

void FriendList::InitGroupList()
{
	sqlPlugin::DataStructDefine dataLib;
	QString strGroup = QString(SELECT_GROUP).arg(*m_pUserNumber);
	sqlPlugin::DataStructDefine& groupdata = GET_DATA(dataLib, strGroup);
	QPixmap pix;
	if (!groupdata.m_lstAllData.isEmpty()) 
		for (int i = 0; i < groupdata.m_lstAllData.size(); i++) {
			QTreeWidgetItem* itemTree = new QTreeWidgetItem(m_pGroupTree);
			QByteArray arrayImage = groupdata.m_lstAllData[i]["IMAGE"].toByteArray();
			QString strName = groupdata .m_lstAllData[i]["GROUP_NAME"].toString();
			if (pix.loadFromData(arrayImage)) {
				QPixmap newpix = pix.scaled(100, 100, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
				newpix = PixmapToRound(newpix, 100);
				CustomToolButton* pToolBu = new CustomToolButton(ui.Friend_List);
				pToolBu->setIcon(newpix);
				pToolBu->setIconSize(QSize(50, 50));
				pToolBu->setText(strName);
				pToolBu->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
				pToolBu->setAutoRaise(true);
				connect(pToolBu, SIGNAL(clicked()), this, SLOT(SlotStartGroupChat()));
				m_mapGroup[pToolBu] = groupdata.m_lstAllData[i]["GROUP_ACCOUNT"].toString();
				pToolBu->setMinimumHeight(60);
				ui.Friend_List->setItemWidget(itemTree, 0, pToolBu);
			}
		}
}

void FriendList::InitQQSpaceList() 
{
	AbstractWidget* pPluginInstance = (AbstractWidget*)GETINSTANCE("QQSpacePlugin");
	if (pPluginInstance) {
		//设置qq空间


	}
}

QPixmap FriendList::PixmapToRound(const QPixmap &src, int radius)
{	
	if (src.isNull()) 
		return QPixmap();
	QSize size(2 * radius, 2 * radius);
	QBitmap mask(size);
	QPainter painter(&mask);
	painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
	painter.fillRect(0, 0, size.width(), size.height(), Qt::white);
	painter.setBrush(QColor(0, 0, 0));
	painter.drawRoundedRect(0, 0, size.width(), size.height(), 99, 99);
	QPixmap image = src.scaled(size, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
	image.setMask(mask);
	return image;
}

void FriendList::ShowUnknownMsgCount(const QString& strTgtNum, bool isFriend)
{
	for (QMap<CustomToolButton *, QString>::iterator it = m_mapMesssage.begin();
		it != m_mapMesssage.end();it++) 
		if (*it == strTgtNum) {
			if (it.key()->GetPaintContent().isEmpty())
				it.key()->SetPaintContent(QString::number(1));
			else {
				bool isOk = 0;
				int num = it.key()->GetPaintContent().toUInt(&isOk);
				if (isOk)
					if (num != 99)
						it.key()->SetPaintContent(QString::number(++num));
					else
						it.key()->SetPaintContent("99+");
			}
			return;
		}
	QString strFriendInfo = QString(SELECT_USER).arg(strTgtNum);
	sqlPlugin::DataStructDefine dataLib;
	sqlPlugin::DataStructDefine& data = GET_DATA(dataLib, strFriendInfo);
	if (!data.m_lstAllData.isEmpty()) {
		QString strName = data.m_lstAllData[0]["USER_NAME"].toString();
		QByteArray array = data.m_lstAllData[0]["IMAGE"].toByteArray();
		SetMessage_ListUi(strTgtNum, strName, array, isFriend);
	}
}

void FriendList::StartVideoChat(const QString& strNum)
{
	SEND_MESSAGE(false, m_pUserNumber, new QString(strNum));  //代表被动接受
	SendSIG(Signal_::LOADPLUG, "VideoChatPlugin");
}

void FriendList::RecvFriendApply(AddInformation* infor)
{
	if (infor->isconsent()) {
		if (infor->type() == AddInformation_TargetType_isFriend) 
			g_FriendList->InitFriendList();
		else 
			g_FriendList->InitGroupList();
		return;
	}
	CustomAddFriendMessageHint* pFriendHint = new CustomAddFriendMessageHint(this);
	pFriendHint->SetLayout(infor);
	QListWidgetItem* lstitem = new QListWidgetItem(ui.listV_Message);
	lstitem->setSizeHint(QSize(ui.listV_Message->width(), 80));
	ui.listV_Message->insertItem(0, lstitem);
	ui.listV_Message->setItemWidget(lstitem, pFriendHint);
}

void FriendList::paintEvent(QPaintEvent *event)
{
	m_pSystemMenu->move(QCursor::pos());
	AbstractWidget::paintEvent(event);
}

void FriendList::UpdateFriendState(const QString strNum, StateInformation state)
{
	for (QMap<CustomToolButton *, QString>::iterator it = m_mapFriend.begin();
		it != m_mapFriend.end();it++) 
		if (*it == strNum) {
			switch (state.currstate())
			{
			case StateInformation::StateMsg::StateInformation_StateMsg_offline:
			case StateInformation::StateMsg::StateInformation_StateMsg_hide:
				it.key()->setIcon(QIcon(QPixmap::fromImage(convertImage(it.key()->icon(), it.key()->iconSize()))));
				break;
			case StateInformation::StateMsg::StateInformation_StateMsg_Online:
			case StateInformation::StateMsg::StateInformation_StateMsg_dontexcuse:
			{
				sqlPlugin::DataStructDefine dataLib;
				sqlPlugin::DataStructDefine& data = GET_DATA(dataLib, QString(SELECT_USER_IMAGE).arg(strNum));
				QByteArray array = data.m_lstAllData[0]["IMAGE"].toByteArray();
				QImage im;
				if (im.loadFromData(array)) 
					it.key()->setIcon(QIcon(PixmapToRound(QPixmap::fromImage(im), 50)));
			}
				break;
			default:
				break;
			}
		}
}

QImage FriendList::convertImage(QIcon iconSource, QSize size)
{
	QImage im = iconSource.pixmap(size).toImage();
	for (int i = 0; i < im.width(); i++)
		for (int j = 0; j < im.height(); j++) {
			QRgb color = im.pixel(i, j);
			im.setPixel(i, j, qRgba(qGray(color), qGray(color), qGray(color), qAlpha(color)));
		}
	return g_FriendList->PixmapToRound(QPixmap::fromImage(im), 50).toImage();
}

void FriendList::SaveChatRecord(protocol& proto)
{
	QString strTragetAccount;
	sqlPlugin::DataStructDefine dataLib, dataLib1;
	switch (proto.type()) {
	case protocol_MsgType_tcp:
	{
		if (proto.has_addinfor())
			strTragetAccount = QString::fromStdString(proto.mutable_addinfor()->fromaccount());
		else
			strTragetAccount = proto.chatcontent(0).targetnumber().c_str();

		QString strChat = QString(SELECT_CHAT_MESSAGE).arg(*m_pUserNumber).arg(strTragetAccount);
		sqlPlugin::DataStructDefine& targetMessage = GET_DATA(dataLib, strChat);
		if (targetMessage.m_lstAllData.isEmpty()) {
			QString strUpdateChat = QString(UPDATE_CHAT_MESSAGE).arg(proto.SerializeAsString().c_str()).arg(*m_pUserNumber).arg(strTragetAccount);
			EXECUTE(strUpdateChat);
			return;
		}
		QString strContent = QString::fromUtf8(targetMessage.m_lstAllData[0]["CHAT_RECORD"].toByteArray().data());
		protocol pro;
		if (pro.ParseFromString(strContent.toStdString())) {
			pro.MergeFrom(proto);
			QString strUpdateChat = QString(UPDATE_CHAT_MESSAGE).arg(pro.SerializeAsString().c_str()).arg(*m_pUserNumber).arg(strTragetAccount);
			EXECUTE(strUpdateChat);
		}
	}
		break;
	case protocol_MsgType_udp:
	{
		QString strChat = QString(SELECT_CHAT_MESSAGE).arg(*m_pUserNumber).arg(proto.group(0).account().c_str());
		sqlPlugin::DataStructDefine& targetMessage = GET_DATA(dataLib1, strChat);
		if (targetMessage.m_lstAllData.isEmpty()) {
			QString strUpdateChat = QString(UPDATE_CHAT_MESSAGE).arg(proto.SerializeAsString().c_str()).arg(*m_pUserNumber).arg(proto.group(0).account().c_str());
			EXECUTE(strUpdateChat);
			return;
		}
		QString strContent = QString::fromUtf8(targetMessage.m_lstAllData[0]["CHAT_RECORD"].toByteArray().data());
		protocol pro;
		if (pro.ParseFromString(strContent.toStdString())) {
			pro.MergeFrom(proto);
			QString strUpdateChat = QString(UPDATE_CHAT_MESSAGE).arg(pro.SerializeAsString().c_str()).arg(*m_pUserNumber).arg(proto.group(0).account().c_str());
			EXECUTE(strUpdateChat);
		}
	}
		break;
	default:
		break;
	}
}

void FriendList::setCurrentState(protocol& proto)
{
	QString strState = g_FriendList->ui.ComState->currentText(); 
	if (strState == QString::fromLocal8Bit("在线"))
		proto.mutable_state()->set_currstate(StateInformation::StateMsg::StateInformation_StateMsg_Online);
	else if (strState == QString::fromLocal8Bit("隐身"))
		proto.mutable_state()->set_currstate(StateInformation::StateMsg::StateInformation_StateMsg_hide);
	else
		proto.mutable_state()->set_currstate(StateInformation::StateMsg::StateInformation_StateMsg_dontexcuse);
}

void FriendList::SlotStartChat()
{
	CustomToolButton* pTgButton = qobject_cast<CustomToolButton*>(sender());
	CustomToolButton* pTgCopyButton = new CustomToolButton(pTgButton);
	Send_MessageThread("ChatMessagePlugin","ChatMessagePlugin" ,SEND_MESSAGE(m_pUserNumber, new QString(m_mapFriend[pTgButton]), pTgCopyButton, true)); //参数为本人账号，对方账号，是否为单聊
	//打开聊天界面，双方号码，昵称，头像
	SendSIG(Signal_::LOADPLUG, "ChatMessagePlugin");
}

void FriendList::SlotStartGroupChat()
{	
	CustomToolButton* pTgButton= qobject_cast<CustomToolButton*>(sender());
	CustomToolButton* pTgCopyButton = new CustomToolButton(pTgButton);
	Send_MessageThread("ChatMessagePlugin", "ChatMessagePlugin", SEND_MESSAGE(m_pUserNumber, new QString(m_mapGroup[pTgButton]), pTgCopyButton, false));//参数为本人账号，对方账号，是否为单聊
	//群号码，名称，成员，以及详细信息，以及所有人的信息
	SendSIG(Signal_::LOADPLUG, "ChatMessagePlugin");
}


void FriendList::SlotStartChatFromMessage()
{
	CustomToolButton* cusToBu = qobject_cast<CustomToolButton*>(sender());
	CustomToolButton* pTgCopyButton = new CustomToolButton(cusToBu);
	Send_MessageThread("ChatMessagePlugin", "ChatMessagePlugin", SEND_MESSAGE(m_pUserNumber, new QString(m_mapMesssage[cusToBu]), pTgCopyButton, true));//参数为本人账号，对方账号，是否为单聊
	if (!PlugIsRuning("ChatMessagePlugin", "ChatMessagePlugin")) 
		//群号码，名称，成员，以及详细信息，以及所有人的信息
		SendSIG(Signal_::LOADPLUG, "ChatMessagePlugin");
}

void FriendList::SlotStartGroupChatFromMessage()
{
	CustomToolButton* cusToBu = qobject_cast<CustomToolButton*>(sender());
	CustomToolButton* pTgCopyButton = new CustomToolButton(cusToBu);
	Send_MessageThread("ChatMessagePlugin", "ChatMessagePlugin", SEND_MESSAGE(m_pUserNumber, new QString(m_mapMesssage[cusToBu]), pTgCopyButton, false));//参数为本人账号，对方账号，是否为单聊
	if (!PlugIsRuning("ChatMessagePlugin", "ChatMessagePlugin"))
		//群号码，名称，成员，以及详细信息，以及所有人的信息
		SendSIG(Signal_::LOADPLUG, "ChatMessagePlugin");
}

void FriendList::SlotAdd(bool isClicked /*= false*/)
{
	QAction* pTgt = qobject_cast<QAction*>(sender());
	if (pTgt->iconText() == QString::fromLocal8Bit("添加好友")) 
		SendSIG(Signal_::SHOW_ABSTRACTWIDGET, new AddFriend(true, this));
	else if (pTgt->iconText() == QString::fromLocal8Bit("添加群")) 
		SendSIG(Signal_::SHOW_ABSTRACTWIDGET, new AddFriend(false, this));
	else if (pTgt->text() == QString::fromLocal8Bit("关于"))
	{
	}
	else if (pTgt->text() == QString::fromLocal8Bit("更换头像")) {
		QString fileName = QFileDialog::getOpenFileName(this, tr("Open Image"), "/", tr("Image Files (*.png *.jpg *.bmp)"), 0, QFileDialog::DontUseNativeDialog);
		ui.LabImage->setPixmap(PixmapToRound(QPixmap(fileName), 40));
		QFile image(fileName);
		if (image.open(QIODevice::ReadOnly)) {
			QByteArray array = image.readAll();
			UPDATE_IMAGE(QString(UPDATEIMAGE).arg("user_account").arg(*m_pUserNumber), QVariant(array));
			QFile fileImage(CONDIGFILE);
			if (fileImage.open(QIODevice::WriteOnly)) {
				fileImage.write(array); 
				fileImage.close();
			}
			image.close();
		}
	}
	else if (pTgt->text() == QString::fromLocal8Bit("编辑资料")) {

	}
	else {

	}
}

void FriendList::SwitchFriMsgSpace()
{
	QPushButton* button = qobject_cast<QPushButton*>(sender());
	if (button == ui.BtnFriend) {
		ui.widget_3->show();
		ui.listV_Message->hide();
		ui.list_Space->hide();
		ui.Friend_List->show();
	}
	else if (button == ui.BtnMessage) {
		ui.widget_3->hide();
		ui.listV_Message->showMaximized();
		ui.list_Space->hide();
		ui.Friend_List->hide();
	}
	else {
		ui.widget_3->hide();
		ui.listV_Message->hide();
		ui.list_Space->showMaximized();
		ui.Friend_List->hide();
	}
}

void FriendList::SlotChangedState(const QString &strCurrentText)
{
	//写数据库
	QString strUpdateState = QString(UPDATESTATE).arg(*m_pUserNumber).arg(strCurrentText);
	EXECUTE(strUpdateState);
	protocol proto;
	proto.set_myselfnum(m_pUserNumber->toStdString());
	if (strCurrentText == QString::fromLocal8Bit("在线"))
		proto.mutable_state()->set_currstate(StateInformation::StateMsg::StateInformation_StateMsg_Online);
	else if(strCurrentText == QString::fromLocal8Bit("隐身"))
		proto.mutable_state()->set_currstate(StateInformation::StateMsg::StateInformation_StateMsg_hide);
	else if (strCurrentText == QString::fromLocal8Bit("离线"))
		proto.mutable_state()->set_currstate(StateInformation::StateMsg::StateInformation_StateMsg_offline);
	else 
		proto.mutable_state()->set_currstate(StateInformation::StateMsg::StateInformation_StateMsg_dontexcuse);

	sqlPlugin::DataStructDefine frienddata;
	GET_DATA(frienddata, QString(SELECT_FRIEND).arg(*m_pUserNumber));
	for (int i = 0;i < frienddata.m_lstAllData.size();i++) {
		std::string* strFriNum = proto.mutable_state()->add_allfriend();
		QString strAccount = frienddata.m_lstAllData[i]["USER_ACCOUNT"].toString();
		if (strAccount == *m_pUserNumber)
			strAccount = frienddata.m_lstAllData[i]["FRIEND_ACCOUNT"].toString();
		std::string strNumber =  strAccount.toStdString();
		strFriNum->assign(strNumber);
	}
	proto.set_client_type(protocol_ClientType_FriendList);
	proto.set_type(protocol_MsgType_stateInfor);
	int size = m_NetWorkProsess->Send(proto.SerializeAsString());
}

CustomToolButton::CustomToolButton(QWidget* parent /*= 0*/) : QToolButton(parent)
{

}

CustomToolButton::CustomToolButton(const CustomToolButton* button)
{
	if (this != button) {
		this->m_strPaintContent = button->m_strPaintContent;
		this->setText(button->text());
		this->setIcon(button->icon());
		this->setIconSize(button->iconSize());
		this->setToolButtonStyle(button->toolButtonStyle());
		this->setAutoRaise(button->autoRaise());
	}
}

const CustomToolButton& CustomToolButton::operator=(const CustomToolButton* button)
{
	if (this != button) {
		this->m_strPaintContent = button->m_strPaintContent;
		this->setText(button->text());
		this->setIcon(button->icon());
		this->setToolButtonStyle(button->toolButtonStyle());
		this->setAutoRaise(button->autoRaise());
	}
	return *this;
}

CustomToolButton::~CustomToolButton()
{

}
#define FONT_SIZE  10
void CustomToolButton::paintEvent(QPaintEvent *event)
{
	QFont f("Microsoft YaHei", FONT_SIZE, QFont::Bold);
	QPainter pa(this);
	pa.setFont(f);
	QPen pen(Qt::red);
	pa.setPen(pen);
	QPoint textPoint = this->rect().center();
	textPoint.setX(this->rect().right() - FONT_SIZE * 3);
	textPoint.setY(textPoint.y() - FONT_SIZE / 2);
	pa.drawText(textPoint, m_strPaintContent);
	QToolButton::paintEvent(event);
}

void CustomToolButton::SetPaintContent(QString strContent)
{
	m_strPaintContent = strContent;
}

QString& CustomToolButton::GetPaintContent()
{
	return m_strPaintContent;
}




CustomAddFriendMessageHint::CustomAddFriendMessageHint(AbstractWidget* parent /*= 0*/) : AbstractWidget(parent), m_UserWidget(NULL)
{

}

CustomAddFriendMessageHint::~CustomAddFriendMessageHint()
{

}

void CustomAddFriendMessageHint::SetLayout(AddInformation* infor)
{
	m_infor = *infor;
	m_IsAddFriend = (infor->type() == AddInformation_TargetType_isFriend);
	sqlPlugin::DataStructDefine dataLib;
	sqlPlugin::DataStructDefine& data = GET_DATA(dataLib, QString(SELECT_USER).arg(infor->fromaccount().c_str()));
	QPushButton* pButtonConsent = new QPushButton(QString::fromLocal8Bit("同意"), this);
	connect(pButtonConsent, SIGNAL(clicked()), this, SLOT(ConsentApply()));
	QVBoxLayout* hBox = new QVBoxLayout;
	CustomToolButton* pBtnTgt = new CustomToolButton(this);
	QImage image;
	image.loadFromData(data.m_lstAllData[0]["IMAGE"].toByteArray());
	m_Gender = data.m_lstAllData[0]["GENDER"].toInt();
	pBtnTgt->setIcon(QPixmap::fromImage(image));
	pBtnTgt->setText(data.m_lstAllData[0]["USER_NAME"].toString());
	pBtnTgt->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
	pBtnTgt->setAutoRaise(true);
	QTextEdit* pReasonText = new QTextEdit(this);
	pReasonText->setText(infor->reason().c_str());
	pReasonText->setReadOnly(true);
	hBox->addWidget(pBtnTgt);
	hBox->addWidget(pReasonText);
	QGridLayout* grid = new QGridLayout;
	grid->addLayout(hBox,0,0);
	grid->addWidget(pButtonConsent,0,1);
	grid->setContentsMargins(0, 0, 0, 0);
	grid->setSpacing(0);
	setLayout(grid);
	connect(pBtnTgt, SIGNAL(clicked()), this, SLOT(UserInfor()));
}

void CustomAddFriendMessageHint::ConsentApply()
{
	QPushButton* button = qobject_cast<QPushButton*>(sender());
	button->setEnabled(false);
	if (m_IsAddFriend) {
		EXECUTE(QString(INSERT_MAP).arg(m_infor.fromaccount().c_str()).arg("").arg(*FriendList::m_pUserNumber));
		g_FriendList->InitFriendList();
	} 
	else {
		EXECUTE(QString(INSERT_MAP).arg("").arg(m_infor.fromaccount().c_str()).arg(*FriendList::m_pUserNumber));
		g_FriendList->InitGroupList();
	}
	protocol proto;
	proto.mutable_addinfor()->set_targetaccount(m_infor.fromaccount());
	proto.mutable_addinfor()->set_fromaccount(m_infor.targetaccount());
	proto.mutable_addinfor()->set_isconsent(true);
	proto.mutable_addinfor()->set_type((AddInformation_TargetType)(int)m_IsAddFriend);
	proto.set_type(protocol_MsgType_tcp);
	FriendList::m_NetWorkProsess->Send(proto.SerializeAsString());
}

void CustomAddFriendMessageHint::UserInfor()
{
	CustomToolButton* pbutton = qobject_cast<CustomToolButton*>(sender());
	if (!m_UserWidget) {
		m_UserWidget = new AbstractWidget(this);
		QLabel* labImage = new QLabel(m_UserWidget);
		QLabel* labUserName = new QLabel(m_UserWidget);
		QLabel* labNumber = new QLabel(m_UserWidget);
		labImage->setPixmap(pbutton->icon().pixmap(300,300));
		labUserName->setText(pbutton->text());
		labNumber->setText(!m_Gender ? QString::fromLocal8Bit("男") : QString::fromLocal8Bit("女"));
		QVBoxLayout* lay = new QVBoxLayout;
		lay->addWidget(labImage);
		lay->addWidget(labUserName);
		lay->addWidget(labNumber);
		labImage->setMinimumSize(300, 300);
		m_UserWidget->setLayout(lay);
		SendSIG(Signal_::SHOW_ABSTRACTWIDGET, m_UserWidget);
	}
	else 
		SendSIG(Signal_::SHOW_ABSTRACTWIDGET, m_UserWidget);
}
