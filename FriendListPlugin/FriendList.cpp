#include "stdafx.h"
#include "MacroDefine.h"
#include "FriendList.h"
#include <QBitmap>
#include <QToolButton>
#include <QGroupBox>

FriendList::FriendList(QWidget *parent)
	: QWidget(parent), m_pUserNumber(nullptr)
{
	ui.setupUi(this);
	ui.list_Space->hide();
	ui.Friend_List->hide();
	const QString* strUserName = (QString *)GET_MESSAGE(0);
	m_pUserNumber = (QString *)GET_MESSAGE(1);
	ui.labUserName->setText(*strUserName);
	delete strUserName;
	strUserName = nullptr; 
	InitFriendList();
	InitGroupList();
	InitQQSpaceList(); 
	RecoveryChatRecord();
}

FriendList::~FriendList()
{

}

void FriendList::RecoveryChatRecord()
{
	ChatRecord chatrecord;
	QMap<QString, ChatRecord> mapChat;
	QString strSelectFriend = QString(SELECT_CHAT_CONTENT).arg(*m_pUserNumber);
	sqlPlugin::DataStructDefine& target = GET_DATA(strSelectFriend);
	for (int i = 0; i < target.m_lstAllData.size();i++) {
		std::string array = QString::fromUtf8(target.m_lstAllData[i]["CHAT_RECORD"].toByteArray().data()).toStdString();
		if (chatrecord.ParseFromString(array) && !array.empty()) {
			QString strAccount = target.m_lstAllData[i]["FRIEND_ACCOUNT"].toString();
			mapChat[strAccount] = chatrecord;
		}
	}

	for (QMap<QString, ChatRecord>::const_iterator it = mapChat.constBegin();
		it != mapChat.constEnd(); it++) {
		QString strSelectUser = QString(SELECT_USER).arg(it.key());
		sqlPlugin::DataStructDefine& data = GET_DATA(strSelectUser);
		if (!data.m_lstAllData.isEmpty()) {
			QString strName = data.m_lstAllData[0]["USER_NAME"].toString();
			QByteArray array = data.m_lstAllData[0]["IMAGE"].toByteArray();
			SetMessage_ListUi(strName, array);
		}
	}
}

void FriendList::SetMessage_ListUi(const QString& strName, const QByteArray& byteImage)
{
	QWidget* pMessageWidget = new QWidget(ui.listV_Message);
	QLabel* pImage = new QLabel(pMessageWidget);
	pImage->setMaximumWidth(50);
	QLabel* pLabName = new QLabel(pMessageWidget);
	QLabel* pLabFirstMessage = new QLabel(pMessageWidget);
	QVBoxLayout* pVBox = new QVBoxLayout(pMessageWidget);
	QHBoxLayout* pHBox = new QHBoxLayout(pMessageWidget);
	pVBox->addWidget(pLabName);
	pVBox->addWidget(pLabFirstMessage);
	pHBox->addWidget(pImage);
	pHBox->addLayout(pVBox);  
	pMessageWidget->setLayout(pHBox);
	QPixmap pix;
	if (pix.loadFromData(byteImage)) {
		QPixmap newpix = pix.scaled(100, 100, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
		newpix = PixmapToRound(newpix, 50);
		pImage->setPixmap(pix);
		pImage->setObjectName("person");
	}
	pLabName->setText(strName);
	QListWidgetItem* lstitem = new QListWidgetItem(ui.listV_Message);
	ui.listV_Message->addItem(lstitem);
	ui.listV_Message->setItemWidget(lstitem, pMessageWidget);
}

void FriendList::paintEvent(QPaintEvent *event)
{
	//计算未读消息的数量，并画在消息消息界面上
}

void FriendList::InitFriendList()
{
	QString strFriend = QString(SELECT_FRIEND).arg(*m_pUserNumber);
	sqlPlugin::DataStructDefine& data = GET_DATA(strFriend);
	QPixmap pix;
	if (!data.m_lstAllData.isEmpty()) {
		QGroupBox* group = new QGroupBox(ui.Friend_List);
		QVBoxLayout* layout = new QVBoxLayout(group);
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
				QToolButton* pToolBu = new QToolButton(ui.Friend_List);
				pToolBu->setIcon(newpix);
				pToolBu->setText(strName);
				pToolBu->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
				pToolBu->setAutoRaise(true);
				connect(pToolBu, SIGNAL(clicked()), this, SLOT(StartChat()));
				m_mapFriend[pToolBu] = friendata.m_lstAllData[0]["USER_ACCOUNT"].toString();
				layout->addWidget(pToolBu);
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
		QVBoxLayout* layout = new QVBoxLayout(group);
		layout->setMargin(20);
		layout->setAlignment(Qt::AlignLeft);
		for (int i = 0; i < groupdata.m_lstAllData.size(); i++) {
			QByteArray arrayImage = groupdata.m_lstAllData[i]["IMAGE"].toByteArray();
			QString strName = groupdata .m_lstAllData[i]["GROUP_NAME"].toString();
			if (pix.loadFromData(arrayImage)) {
				QPixmap newpix = pix.scaled(100, 100, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
				newpix = PixmapToRound(newpix, 50);
				QToolButton* pToolBu = new QToolButton(ui.Friend_List);
				pToolBu->setIcon(newpix);
				pToolBu->setText(strName);
				pToolBu->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
				pToolBu->setAutoRaise(true);
				connect(pToolBu, SIGNAL(clicked()), this, SLOT(StartGroupChat()));
				m_mapGroup[pToolBu] = groupdata.m_lstAllData[i]["GROUP_ACCOUNT"].toString();
				layout->addWidget(pToolBu);
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

void FriendList::StartChat()
{
	QToolButton* pTgButton = qobject_cast<QToolButton*>(sender());
	SEND_MESSAGE(m_pUserNumber, new QString(m_mapFriend[pTgButton]), true); 
	//打开聊天界面，双方号码，昵称，头像
	SEND_SIGNAL(Signal_::LOADPLUG, "ChatMessagePlugin", true);
}

void FriendList::StartGroupChat()
{	
	QToolButton* pTgButton= qobject_cast<QToolButton*>(sender());
	SEND_MESSAGE(m_pUserNumber, new QString(m_mapGroup[pTgButton]), false);
	//群号码，名称，成员，以及详细信息，以及所有人的信息
	SEND_SIGNAL(Signal_::LOADPLUG, "ChatMessagePlugin");
}
