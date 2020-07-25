#include "stdafx.h"
#include "MacroDefine.h"
#include "FriendList.h"
#include "AddFriend.h"

FriendList* g_FriendList = NULL;
QString* FriendList::m_pUserNumber = NULL;
#define    CONDIGFILE   "../Data/Image/Avatar.jpg"
FriendList::FriendList(QWidget *parent) : AbstractWidget(parent), m_pSystemMenu(NULL)
{
	ui.setupUi(this);
	g_FriendList = this;
	ui.list_Space->hide();
	ui.widget_3->hide();
	const QString* strUserName = (QString *)GET_MESSAGE(0);
	m_pUserNumber = (QString *)GET_MESSAGE(1);
	ui.labUserName->setText(*strUserName);
	delete strUserName;
	strUserName = nullptr; 
	QHostAddress host("192.168.1.17");
	ProsessMessage* pNetMsg = new ProsessMessage(AbstractNetWork::ProtoType::TCP, host, 7007, this);
	SendSIG(Signal_::INITIALIZENETWORK, pNetMsg);
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
	ui.LabImage->setPixmap(PixmapToRound(QPixmap(CONDIGFILE), 40));
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
	protocol protoContent;
	QMap<QString, protocol> mapChat;
	QString strSelectFriend = QString(SELECT_CHAT_CONTENT).arg(*m_pUserNumber);
	sqlPlugin::DataStructDefine& target = GET_DATA(strSelectFriend);
	for (int i = 0; i < target.m_lstAllData.size();i++) {
		std::string array = QString::fromUtf8(target.m_lstAllData[i]["CHAT_RECORD"].toByteArray().data()).toStdString();
		if (protoContent.ParseFromString(array) && !array.empty()) {
			QString strAccount = target.m_lstAllData[i]["FRIEND_ACCOUNT"].toString();
			mapChat[strAccount] = protoContent;
		}
	}

	for (QMap<QString, protocol>::const_iterator it = mapChat.constBegin();
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
		isFriend ? connect(pCusBu, SIGNAL(clicked()), this, SLOT(SlotStartChatFromMessage())) : connect(pCusBu, SIGNAL(clicked()), this, SLOT(SlotStartGroupChatFromMessage()));
		m_mapMesssage[pCusBu] = strNum;
	}
	QListWidgetItem* lstitem = new QListWidgetItem(ui.listV_Message);
	ui.listV_Message->addItem(lstitem);
	ui.listV_Message->setItemWidget(lstitem, pCusBu);
}

void FriendList::InitFriendList()
{
	ui.Friend_List->removeItem(0);
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
				connect(pToolBu, SIGNAL(clicked()), this, SLOT(SlotStartChat()));
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
	ui.Friend_List->removeItem(1);
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
				connect(pToolBu, SIGNAL(clicked()), this, SLOT(SlotStartGroupChat()));
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
	painter.setRenderHint(QPainter::Antialiasing);
	painter.setRenderHint(QPainter::SmoothPixmapTransform);
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

void FriendList::RecvFriendApply(AddInformation* infor)
{
	CustomAddFriendMessageHint* pFriendHint = new CustomAddFriendMessageHint(this);
	pFriendHint->SetLayout(infor);
}

void FriendList::paintEvent(QPaintEvent *event)
{
	m_pSystemMenu->move(QCursor::pos());
	AbstractWidget::paintEvent(event);
}

void FriendList::SlotStartChat()
{
	CustomToolButton* pTgButton = qobject_cast<CustomToolButton*>(sender());
	Send_MessageThread("ChatMessagePlugin","ChatMessagePlugin" ,SEND_MESSAGE(m_pUserNumber, new QString(m_mapFriend[pTgButton]), pTgButton, true)); //参数为本人账号，对方账号，是否为单聊
	//打开聊天界面，双方号码，昵称，头像
	SendSIG(Signal_::LOADPLUG, "ChatMessagePlugin");
}

void FriendList::SlotStartGroupChat()
{	
	CustomToolButton* pTgButton= qobject_cast<CustomToolButton*>(sender());
	Send_MessageThread("ChatMessagePlugin", "ChatMessagePlugin", SEND_MESSAGE(m_pUserNumber, new QString(m_mapGroup[pTgButton]), pTgButton, false));//参数为本人账号，对方账号，是否为单聊
	//群号码，名称，成员，以及详细信息，以及所有人的信息
	SendSIG(Signal_::LOADPLUG, "ChatMessagePlugin");
}


void FriendList::SlotStartChatFromMessage()
{
	CustomToolButton* cusToBu = qobject_cast<CustomToolButton*>(sender());
	Send_MessageThread("ChatMessagePlugin", "ChatMessagePlugin", SEND_MESSAGE(m_pUserNumber, new QString(m_mapMesssage[cusToBu]), cusToBu, true));//参数为本人账号，对方账号，是否为单聊
	if (!PlugIsRuning("ChatMessagePlugin", "ChatMessagePlugin")) 
		//群号码，名称，成员，以及详细信息，以及所有人的信息
		SendSIG(Signal_::LOADPLUG, "ChatMessagePlugin");
}

void FriendList::SlotStartGroupChatFromMessage()
{
	CustomToolButton* cusToBu = qobject_cast<CustomToolButton*>(sender());
	Send_MessageThread("ChatMessagePlugin", "ChatMessagePlugin", SEND_MESSAGE(m_pUserNumber, new QString(m_mapMesssage[cusToBu]), cusToBu, false));//参数为本人账号，对方账号，是否为单聊
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
		QString fileName = QFileDialog::getOpenFileName(this, tr("Open Image"), "/", tr("Image Files (*.png *.jpg *.bmp)"));
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
	}
	else if (button == ui.BtnMessage) {
		ui.widget_3->hide();
		ui.listV_Message->showMaximized();
		ui.list_Space->hide();
	}
	else {
		ui.widget_3->hide();
		ui.listV_Message->hide();
		ui.list_Space->showMaximized();
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




CustomAddFriendMessageHint::CustomAddFriendMessageHint(AbstractWidget* parent /*= 0*/) : AbstractWidget(parent), m_UserWidget(NULL)
{

}

CustomAddFriendMessageHint::~CustomAddFriendMessageHint()
{

}

void CustomAddFriendMessageHint::SetLayout(AddInformation* infor)
{
	m_IsAddFriend = (infor->type() == AddInformation_TargetType_isFriend);
	sqlPlugin::DataStructDefine& data = GET_DATA(QString(SELECT_USER).arg(infor->fromaccount().c_str()));
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
		EXECUTE(QString(INSERT_MAP).arg(button->text()).arg("").arg(*FriendList::m_pUserNumber));
		g_FriendList->InitFriendList();
	} 
	else {
		EXECUTE(QString(INSERT_MAP).arg("").arg(button->text()).arg(*FriendList::m_pUserNumber));
		g_FriendList->InitGroupList();
	}
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
		labNumber->setText(m_Gender ? QString::fromLocal8Bit("男") : QString::fromLocal8Bit("女"));
		QVBoxLayout* lay = new QVBoxLayout;
		lay->addWidget(labImage);
		lay->addWidget(labUserName);
		lay->addWidget(labNumber);
		labImage->setMinimumSize(300, 300);
		m_UserWidget->setLayout(lay);
		SendSIG(Signal_::SHOW_ABSTRACTWIDGET, m_UserWidget);
	}
}
