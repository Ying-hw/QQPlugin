#include "stdafx.h"
#include "MacroDefine.h"
#include "FriendList.h"
#include "ProsessMessage.h"


FriendList* g_FriendList = NULL;

FriendList::FriendList(QWidget *parent)
	: AbstractWidget(parent), m_pUserNumber(nullptr)
{
	ui.setupUi(this);
	g_FriendList = this;
	ui.list_Space->hide();
	ui.Friend_List->hide();
	const QString* strUserName = (QString *)GET_MESSAGE(0);
	m_pUserNumber = (QString *)GET_MESSAGE(1);
	ui.labUserName->setText(*strUserName);
	delete strUserName;
	strUserName = nullptr; 
	QHostAddress host("192.168.1.17");
	ProsessMessage* promess = new ProsessMessage(AbstractNetWork::ProtoType::TCP, host, 7007, this);
	SendSIG(Signal_::INITIALIZENETWORK, promess);
	InitFriendList();
	InitGroupList();
	InitQQSpaceList(); 
	RecoveryChatRecord();
	ui.BtnAdd->setContextMenuPolicy(Qt::CustomContextMenu);
	QAction* pAction_enter = new QAction(QString::fromLocal8Bit("添加好友"));
	QAction* pAction_No_Enter = new QAction(QString::fromLocal8Bit("添加群"));
	QMenu* pMenu = new QMenu(this);
	pMenu->addAction(pAction_enter);
	pMenu->addAction(pAction_No_Enter);
	ui.BtnAdd->setMenu(pMenu);
}

FriendList::~FriendList()
{

}

void FriendList::RecoveryChatRecord()
{
	protocolType protoContent;
	QMap<QString, protocolType> mapChat;
	QString strSelectFriend = QString(SELECT_CHAT_CONTENT).arg(*m_pUserNumber);
	sqlPlugin::DataStructDefine& target = GET_DATA(strSelectFriend);
	for (int i = 0; i < target.m_lstAllData.size();i++) {
		std::string array = QString::fromUtf8(target.m_lstAllData[i]["CHAT_RECORD"].toByteArray().data()).toStdString();
		if (protoContent.ParseFromString(array) && !array.empty()) {
			QString strAccount = target.m_lstAllData[i]["FRIEND_ACCOUNT"].toString();
			mapChat[strAccount] = protoContent;
		}
	}

	for (QMap<QString, protocolType>::const_iterator it = mapChat.constBegin();
		it != mapChat.constEnd(); it++) {
		QString strSelectUser = QString(SELECT_USER).arg(it.key());
		sqlPlugin::DataStructDefine& data = GET_DATA(strSelectUser);
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
		isFriend ? connect(pCusBu, SIGNAL(clicked()), this, SLOT(StartChatFromMessage())) : connect(pCusBu, SIGNAL(clicked()), this, SLOT(StartGroupChatFromMessage()));
		m_mapMesssage[pCusBu] = strNum;
	}
	QListWidgetItem* lstitem = new QListWidgetItem(ui.listV_Message);
	ui.listV_Message->addItem(lstitem);
	ui.listV_Message->setItemWidget(lstitem, pCusBu);
}

void FriendList::InitFriendList()
{
	QString strFriend = QString(SELECT_FRIEND).arg(*m_pUserNumber);
	sqlPlugin::DataStructDefine& data = GET_DATA(strFriend);
	QPixmap pix;
	if (!data.m_lstAllData.isEmpty()) {
		QGroupBox* group = new QGroupBox(ui.Friend_List);
		QHBoxLayout* layout = new QHBoxLayout(group);
		layout->setMargin(20);
		layout->setAlignment(Qt::AlignLeft);
		for (int i = 0; i < data.m_lstAllData.size(); i++) {
			QString strFriendInfo = QString(SELECT_USER).arg(data.m_lstAllData[i]["FRIEND_ACCOUNT"].toString());
			sqlPlugin::DataStructDefine& friendata = GET_DATA(strFriendInfo);
			QByteArray arrayImage = friendata.m_lstAllData[0]["IMAGE"].toByteArray();
			QString strName = friendata.m_lstAllData[0]["USER_NAME"].toString();
			if (pix.loadFromData(arrayImage)) {
				QPixmap newpix = pix.scaled(100, 100, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
				newpix = PixmapToRound(newpix, 50);
				CustomToolButton* pToolBu = new CustomToolButton(ui.Friend_List);
				pToolBu->setIcon(newpix);
				pToolBu->setText(strName);
				pToolBu->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
				pToolBu->setAutoRaise(true);
				connect(pToolBu, SIGNAL(clicked()), this, SLOT(StartChat()));
				m_mapFriend[pToolBu] = friendata.m_lstAllData[0]["USER_ACCOUNT"].toString();
				layout->addWidget(pToolBu);
				group->setLayout(layout);
			}
		}
		ui.Friend_List->addItem((QWidget*)group, QString::fromLocal8Bit("好友列表"));
	}
}

void FriendList::InitGroupList()
{
	QString strGroup = QString(SELECT_GROUP).arg(*m_pUserNumber);
	sqlPlugin::DataStructDefine& groupdata = GET_DATA(strGroup);
	QPixmap pix;
	if (!groupdata.m_lstAllData.isEmpty()) {
		QGroupBox* group = new QGroupBox(ui.Friend_List);
		QHBoxLayout* layout = new QHBoxLayout(group);
		layout->setMargin(20);
		layout->setAlignment(Qt::AlignLeft);
		for (int i = 0; i < groupdata.m_lstAllData.size(); i++) {
			QByteArray arrayImage = groupdata.m_lstAllData[i]["IMAGE"].toByteArray();
			QString strName = groupdata .m_lstAllData[i]["GROUP_NAME"].toString();
			if (pix.loadFromData(arrayImage)) {
				QPixmap newpix = pix.scaled(100, 100, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
				newpix = PixmapToRound(newpix, 50);
				CustomToolButton* pToolBu = new CustomToolButton(ui.Friend_List);
				pToolBu->setIcon(newpix);
				pToolBu->setText(strName);
				pToolBu->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
				pToolBu->setAutoRaise(true);
				connect(pToolBu, SIGNAL(clicked()), this, SLOT(StartGroupChat()));
				m_mapGroup[pToolBu] = groupdata.m_lstAllData[i]["GROUP_ACCOUNT"].toString();
				layout->addWidget(pToolBu);
				group->setLayout(layout);
			}
		}
		ui.Friend_List->addItem((QWidget*)group, QString::fromLocal8Bit("群组"));
	}
}

void FriendList::InitQQSpaceList() 
{
	QWidget* pPluginInstance = (QWidget*)GETINSTANCE("QQSpacePlugin");
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
	painter.setRenderHint(QPainter::Antialiasing);
	painter.setRenderHint(QPainter::SmoothPixmapTransform);
	painter.fillRect(0, 0, size.width(), size.height(), Qt::white);
	painter.setBrush(QColor(0, 0, 0));
	painter.drawRoundedRect(0, 0, size.width(), size.height(), 99, 99);
	QPixmap image = src.scaled(size);
	image.setMask(mask);
	return image;
}

void FriendList::ShowUnknownMsgCount(const QString& strTgtNum, bool isFriend)
{
	for (QMap<CustomToolButton *, QString>::iterator it = m_mapMesssage.begin();
		it != m_mapMesssage.end();it++) {
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
	}
	QString strFriendInfo = QString(SELECT_USER).arg(strTgtNum);
	sqlPlugin::DataStructDefine& data = GET_DATA(strFriendInfo);
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

void FriendList::StartChat()
{
	CustomToolButton* pTgButton = qobject_cast<CustomToolButton*>(sender());
	Send_MessageThread("ChatMessagePlugin","ChatMessagePlugin" ,SEND_MESSAGE(m_pUserNumber, new QString(m_mapFriend[pTgButton]), pTgButton, true)); //参数为本人账号，对方账号，是否为单聊
	//打开聊天界面，双方号码，昵称，头像
	SendSIG(Signal_::LOADPLUG, "ChatMessagePlugin");
}

void FriendList::StartGroupChat()
{	
	CustomToolButton* pTgButton= qobject_cast<CustomToolButton*>(sender());
	Send_MessageThread("ChatMessagePlugin", "ChatMessagePlugin", SEND_MESSAGE(m_pUserNumber, new QString(m_mapGroup[pTgButton]), pTgButton, false));//参数为本人账号，对方账号，是否为单聊
	//群号码，名称，成员，以及详细信息，以及所有人的信息
	SendSIG(Signal_::LOADPLUG, "ChatMessagePlugin");
}


void FriendList::StartChatFromMessage()
{
	CustomToolButton* cusToBu = qobject_cast<CustomToolButton*>(sender());
	Send_MessageThread("ChatMessagePlugin", "ChatMessagePlugin", SEND_MESSAGE(m_pUserNumber, new QString(m_mapMesssage[cusToBu]), cusToBu, true));//参数为本人账号，对方账号，是否为单聊
	if (!PlugIsRuning("ChatMessagePlugin", "ChatMessagePlugin"))
	{
		//群号码，名称，成员，以及详细信息，以及所有人的信息
		SendSIG(Signal_::LOADPLUG, "ChatMessagePlugin");
	}
}

void FriendList::StartGroupChatFromMessage()
{
	CustomToolButton* cusToBu = qobject_cast<CustomToolButton*>(sender());
	Send_MessageThread("ChatMessagePlugin", "ChatMessagePlugin", SEND_MESSAGE(m_pUserNumber, new QString(m_mapMesssage[cusToBu]), cusToBu, false));//参数为本人账号，对方账号，是否为单聊
	if (!PlugIsRuning("ChatMessagePlugin", "ChatMessagePlugin"))
	{
		//群号码，名称，成员，以及详细信息，以及所有人的信息
		SendSIG(Signal_::LOADPLUG, "ChatMessagePlugin");
	}
}

CustomToolButton::CustomToolButton(QWidget* parent /*= 0*/) : QToolButton(parent)
{

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

