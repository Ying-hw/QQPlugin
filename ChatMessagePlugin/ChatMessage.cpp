#include "stdafx.h"
#include "ChatMessage.h"
#include "ProcessChatMessage.h"
#include "SqlStatementDefine.h"
#include "NetProtocConfig.pb.h"
#include "CustomTextEdit.h"
#include "HintFrameWidget.h"

#define LAYOUT_MESSAEG_WIDGET   10
#define TEMP_FILE  "../Data/Config/temp"

ChatMessage* g_pChatMessage = NULL;
ChatMessage::ChatMessage(QWidget *parent) : AbstractWidget(parent), m_ProMsg(NULL)
{
	ui.setupUi(this);
	g_pChatMessage = this;
	ui.TexEditContent->resize(ui.TexEditContent->width(), 80);
	QHostAddress host("192.168.1.17");
	m_ProMsg = new ProcessChatMessage(AbstractNetWork::ProtoType::TCP, host, 7007, this);
	SendSIG(Signal_::INITIALIZENETWORK, m_ProMsg);
	QString* strSelfNum = (QString *)GET_MESSAGE(0); 
	QString* strTargetNum = (QString *)GET_MESSAGE(1);
	CustomToolButton* pToolBu = (CustomToolButton*)GET_MESSAGE(2);
	bool bType = (bool)GET_MESSAGE(3);
	NumInfo num(bType, true, *strTargetNum, 0);
	m_mapFriendState[pToolBu] = num;
	m_strSelfNum = *strSelfNum;
	SetAddChatTgt(pToolBu, *strSelfNum, *strTargetNum);
	m_mapFriendToTextEdit[pToolBu] = ui.TexEditContent;
	QString strChat = QString(SELECT_CHAT_MESSAGE).arg(*strSelfNum).arg(*strTargetNum);
	DataStructDefine& data = GET_DATA(strChat);
	ui.BtnEnter->setContextMenuPolicy(Qt::CustomContextMenu);
	QAction* pAction_enter = new QAction(QString::fromLocal8Bit("按Enter发送消息"));
	QAction* pAction_No_Enter = new QAction(QString::fromLocal8Bit("按Ctrl + Enter发送消息"));
	QMenu* pMenu = new QMenu(this);
	pMenu->addAction(pAction_enter);
	pMenu->addAction(pAction_No_Enter);
	ui.BtnEnter->setMenu(pMenu);
    connect(ui.BtnSend, SIGNAL(clicked()), this, SLOT(SlotSendTextContent()));
	connect(ui.LstFriend, SIGNAL(currentItemChanged(QListWidgetItem *, QListWidgetItem *)), this, SLOT(SlotSwitchFriend(QListWidgetItem *, QListWidgetItem *)));
	connect(ui.BtnFace, SIGNAL(clicked()), this, SLOT(SlotBtnFace()));
	connect(ui.BtnMail, SIGNAL(clicked()), this, SLOT(SlotBtnMail()));
	connect(ui.BtnScreenshot, SIGNAL(clicked()), this, SLOT(SlotBtnScreenshot()));
	connect(ui.BtnVedio, SIGNAL(clicked()), this, SLOT(SlotBtnVideo()));
	connect(ui.BtnVibration, SIGNAL(clicked()), this, SLOT(SlotBtnVibration()));
	connect(ui.BtnVoice, SIGNAL(clicked()), this, SLOT(SlotBtnVoice()));
	connect(ui.BtnVoiceChat, SIGNAL(clicked()), this, SLOT(SlotBtnVoiceChat()));
	connect(ui.BtnEnter, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(SlotCustomMenu(const QPoint&)));
	connect(pAction_enter, SIGNAL(toggled(bool)), this, SLOT(SlotBindEnter(bool)));
	connect(pAction_No_Enter, SIGNAL(toggled(bool)), this, SLOT(SlotRemoveBindEnter(bool)));
	connect(ui.BtnClear, &QPushButton::clicked, [this]() {
		CustomToolButton* pTgtBu = static_cast<CustomToolButton*>(ui.LstFriend->itemWidget(ui.LstFriend->currentItem()));
		m_mapFriendToTextEdit[pTgtBu]->clear();
		m_mapFriendToTextEdit[pTgtBu]->setReadOnly(false);
	});
	m_mapNumberToTable[*strTargetNum] = ui.tab_MessageContent;
	InitChatMessage(data, ui.tab_MessageContent);
	ui.TexEditContent->setFocus();
	ui.widget_2->setFixedHeight(50);
}

ChatMessage::~ChatMessage()
{ 
	SaveChatRecord();
}

void ChatMessage::InitChatMessage(const DataStructDefine& targetMessage, QTableWidget* tab)
{
	QMap<quint64, QList<Message_Content>> mapMessage;
	QList<Message_Content> currentContent;
	QString strContent = QString::fromUtf8(targetMessage.m_lstAllData[0]["CHAT_RECORD"].toByteArray().data());
	protocol proto;
	if (proto.ParseFromString(strContent.toStdString())) {
		for (int i = 0; i < proto.chatcontent_size(); i++) {
			std::string message_content = proto.chatcontent(i).content();
			Message_Content Message;
			Message.type = (Message_Content::Content_Type)proto.chatcontent(i).type();
			Message.strContent = QString::fromStdString(message_content);
			Message.is_Self = proto.chatcontent(i).isself();
			currentContent << Message;
			quint64 message_time = proto.chatcontent(i).time();
			if (!mapMessage.contains(message_time))
				currentContent.clear();
			mapMessage[message_time] = currentContent;
			QList<quint64> time;
			time << message_time;
			m_mapNumberToTime[targetMessage.m_lstAllData[0]["FRIEND_ACCOUNT"].toString()] = time;
		}
	}
	InitMessageUI(mapMessage, tab);
}

void ChatMessage::InitMessageUI(const QMap<quint64, QList<Message_Content>>& targetContent, QTableWidget* tab)
{
	int RowCount = 0, Row = 0;
    for (auto it = targetContent.end(); it != targetContent.begin(); it--) 
        RowCount += it->size();
	tab->setRowCount(RowCount);
	tab->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeMode::Stretch);
	tab->resizeRowsToContents();
	for (auto it = targetContent.end(); it != targetContent.begin();it--) {
		tab->setItem(Row, 0, new QTableWidgetItem(QDateTime::fromMSecsSinceEpoch(it.key()).toString("MM-dd hh:mm:ss")));
		tab->item(Row, 0)->setTextAlignment(Qt::AlignHCenter);
        Row++;
		for (auto it_ = (*it).begin(); it_ != (*it).end();it_++) {
            CustomMessageWidget* MessWidget = new CustomMessageWidget(tab);
			tab->setCellWidget(Row, 0, MessWidget);
			MessWidget->SetContent(it_->strContent, it_->is_Self, (CustomMessageWidget::ContentType)(it_->type));
            Row++;
		}
	}
}

void ChatMessage::SetAddMessage(const QString strTgtNum, const QString strMsg, quint64 time, Message_Content::Content_Type msgtype)
{
	for (QMap<CustomToolButton*, NumInfo>::iterator it = m_mapFriendState.begin();
		it != m_mapFriendState.end(); it++) 
		if (it->m_strNum == strTgtNum && !(it->m_isCurrent)) { //如果找到对应的对方账号的话，且不是当前正在聊天的账号，就把消息加一
			it->m_unknowMessage++;
			it.key()->SetPaintContent(QString::number(it->m_unknowMessage));
		}
	if (m_mapNumberToTime.contains(strTgtNum) && m_mapNumberToTime[strTgtNum].last() != time) {  //如果最后的时间不等于将要接受的时间的话就显示这个新的时间
		QTableWidgetItem* item = new QTableWidgetItem();
		item->setTextAlignment(Qt::AlignCenter);
		item->setText(QDateTime::fromMSecsSinceEpoch(time).toString("MM-dd hh:mm:ss"));
		m_mapNumberToTable[strTgtNum]->setItem(m_mapNumberToTable[strTgtNum]->rowCount() - 1, 0, item);
	}
	CustomMessageWidget* MessWidget = new CustomMessageWidget();
	MessWidget->SetContent(strMsg, false, (CustomMessageWidget::ContentType)msgtype);
	m_mapNumberToTable[strTgtNum]->setCellWidget(m_mapNumberToTable[strTgtNum]->rowCount() - 1, 0, MessWidget);
	m_MsgSourceNum[MessWidget] = strTgtNum;
}

void ChatMessage::SetAddChatTgt(CustomToolButton* pToolTgt, const QString& strSelfNum, const QString& strTgtNum)
{
	QListWidgetItem* pItemWidget = new QListWidgetItem(ui.LstFriend);
	ui.LstFriend->addItem(pItemWidget);
	ui.LstFriend->setItemWidget(pItemWidget, pToolTgt);
	if (ui.LstFriend->count() > 1) {
		QWidget* page = new QWidget(this);
		page->setObjectName("page");
		QGridLayout* gridLayout_7 = new QGridLayout(page);
		gridLayout_7->setSpacing(0);
		gridLayout_7->setContentsMargins(11, 11, 11, 11);
		gridLayout_7->setObjectName("gridLayout_7");
		gridLayout_7->setContentsMargins(0, 0, 0, 0);
		QWidget* widget_3 = new QWidget(page);
		widget_3->setObjectName("widget_3");
		QGridLayout* gridLayout = new QGridLayout(widget_3);
		gridLayout->setSpacing(6);
		gridLayout->setContentsMargins(11, 11, 11, 11);
		gridLayout->setObjectName("gridLayout");
		QLabel* labName = new QLabel(widget_3);
		labName->setObjectName("labName");
		QHBoxLayout *horizontalLayout_2 = new QHBoxLayout();
		horizontalLayout_2->setSpacing(6);
		horizontalLayout_2->addWidget(labName);
		QSpacerItem* horizontalSpacer_2 = new QSpacerItem(37, 17, QSizePolicy::Expanding, QSizePolicy::Minimum);
		horizontalLayout_2->addItem(horizontalSpacer_2);
		gridLayout->addLayout(horizontalLayout_2, 0, 0, 1, 1);
		QHBoxLayout * horizontalLayout_3 = new QHBoxLayout();
		horizontalLayout_3->setSpacing(6);
		QPushButton* pushButton_5 = new QPushButton(widget_3);
		connect(pushButton_5, SIGNAL(clicked()), this, SLOT(SlotBtnVideo()));
		pushButton_5->setObjectName("pushButton_5");
		pushButton_5->setFlat(true);
		QIcon icon4;
		icon4.addFile(QString::fromUtf8("../x64/Data/Image/23_toolbar_down@2x.png"), QSize(), QIcon::Normal, QIcon::Off);
		pushButton_5->setIcon(icon4);
		pushButton_5->setIconSize(QSize(20, 20));
		horizontalLayout_3->addWidget(pushButton_5);

		QPushButton* pushButton_6 = new QPushButton(widget_3);
		connect(pushButton_6, SIGNAL(clicked()), this, SLOT(SlotBtnVoiceChat()));
		pushButton_6->setObjectName("pushButton_6");
		pushButton_6->setFlat(true);
		QIcon icon5;
		icon5.addFile(QString::fromUtf8("../x64/Data/Image/1104753436_toolbar_down@2x.png"), QSize(), QIcon::Normal, QIcon::Off);
		pushButton_6->setIcon(icon5);
		pushButton_6->setIconSize(QSize(20, 20));
		horizontalLayout_3->addWidget(pushButton_6);

		QPushButton* pushButton_7 = new QPushButton(widget_3);
		connect(pushButton_7, SIGNAL(clicked()), this, SLOT(SlotBtnMail()));
		pushButton_7->setObjectName("pushButton_7");
		pushButton_7->setFlat(true);
		QIcon icon6;
		icon6.addFile(QString::fromUtf8("../x64/Data/Image/1_toolbar_default_down@2x.png"), QSize(), QIcon::Normal, QIcon::Off);
		pushButton_7->setIcon(icon6);
		pushButton_7->setIconSize(QSize(20, 20));
		horizontalLayout_3->addWidget(pushButton_7);

		QSpacerItem* horizontalSpacer_4 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
		horizontalLayout_3->addItem(horizontalSpacer_4);
		gridLayout->addLayout(horizontalLayout_3, 1, 0, 1, 1);
		gridLayout_7->addWidget(widget_3, 0, 0, 1, 1);
		QSplitter* splitter_3 = new QSplitter(page);
		splitter_3->setObjectName("splitter_3");
		splitter_3->setLineWidth(0);
		splitter_3->setOrientation(Qt::Vertical);
		splitter_3->setHandleWidth(0);
		splitter_3->setChildrenCollapsible(false);
		QTableWidget* tab_MessageContent = new QTableWidget(splitter_3);
		tab_MessageContent->setColumnCount(1);
		tab_MessageContent->setObjectName(QString::fromUtf8("tab_MessageContent"));
		tab_MessageContent->setFrameShape(QFrame::VLine);
		tab_MessageContent->setFrameShadow(QFrame::Plain);
		tab_MessageContent->setLineWidth(1);
		tab_MessageContent->setMidLineWidth(0);
		tab_MessageContent->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
		tab_MessageContent->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
		tab_MessageContent->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
		tab_MessageContent->setSelectionMode(QAbstractItemView::NoSelection);
		tab_MessageContent->setEditTriggers(QAbstractItemView::NoEditTriggers);
		tab_MessageContent->setShowGrid(false);
		splitter_3->addWidget(tab_MessageContent);
		tab_MessageContent->horizontalHeader()->setVisible(false);
		tab_MessageContent->verticalHeader()->setVisible(false);
		m_mapNumberToTable[strTgtNum] = tab_MessageContent;
		QString strChat = QString(SELECT_CHAT_MESSAGE).arg(strSelfNum).arg(strTgtNum);
		DataStructDefine& data = GET_DATA(strChat);
		InitChatMessage(data, tab_MessageContent);
		QWidget* layoutWidget1 = new QWidget(splitter_3);
		layoutWidget1->setObjectName("layoutWidget1");
		QGridLayout* gridLayout_3 = new QGridLayout(layoutWidget1);
		gridLayout_3->setSpacing(6);
		gridLayout_3->setContentsMargins(11, 11, 11, 11);
		gridLayout_3->setObjectName("gridLayout_3");
		gridLayout_3->setVerticalSpacing(0);
		gridLayout_3->setContentsMargins(0, 0, 0, 0);
		QWidget* widget_2 = new QWidget(layoutWidget1);
		widget_2->setObjectName("widget_2");
		widget_2->setStyleSheet("background-color: rgb(255, 255, 255);");
		ui.widget_2->setFixedHeight(50);
		QGridLayout* gridLayout_2 = new QGridLayout(widget_2);
		gridLayout_2->setSpacing(6);
		gridLayout_2->setObjectName("gridLayout_2");
		gridLayout_2->setContentsMargins(0, 0, 0, 0);
		QPushButton* pushButton = new QPushButton(widget_2);
		connect(pushButton, SIGNAL(clicked()), this, SLOT(SlotBtnFace()));
		pushButton->setObjectName("pushButton");
		pushButton->setFlat(true);
		QIcon icon;
		icon.addFile(QString::fromUtf8("../x64/Data/Image/Face happy.png"), QSize(), QIcon::Normal, QIcon::Off);
		pushButton->setIcon(icon);
		pushButton->setIconSize(QSize(16, 16));

		gridLayout_2->addWidget(pushButton, 0, 0, 1, 1);
		QPushButton* pushButton_2 = new QPushButton(widget_2);
		connect(pushButton_2, SIGNAL(clicked()), this, SLOT(SlotBtnScreenshot()));
		pushButton_2->setObjectName("pushButton_2");
		pushButton_2->setCheckable(false);
		pushButton_2->setAutoRepeat(false);
		pushButton_2->setAutoExclusive(false);
		pushButton_2->setFlat(true);
		QIcon icon1;
		icon1.addFile(QString::fromUtf8("../x64/Data/Image/Clipboard cut.png"), QSize(), QIcon::Normal, QIcon::Off);
		pushButton_2->setIcon(icon1);
		pushButton_2->setIconSize(QSize(16, 16));

		gridLayout_2->addWidget(pushButton_2, 0, 1, 1, 1);
		QPushButton* pushButton_3 = new QPushButton(widget_2);
		connect(pushButton_3, SIGNAL(clicked()), this, SLOT(SlotBtnVibration()));
		pushButton_3->setObjectName("pushButton_3");
		pushButton_3->setFlat(true);
		QIcon icon2;
		icon2.addFile(QString::fromUtf8("../x64/Data/Image/My images1.png"), QSize(), QIcon::Normal, QIcon::Off);
		pushButton_3->setIcon(icon2);
		pushButton_3->setIconSize(QSize(16, 16));
		
		gridLayout_2->addWidget(pushButton_3, 0, 2, 1, 1);
		QPushButton* pushButton_4 = new QPushButton(widget_2);
		connect(pushButton_4, SIGNAL(clicked()), this, SLOT(SlotBtnVoice()));
		pushButton_4->setObjectName("pushButton_4");
		pushButton_4->setFlat(true);
		QIcon icon3;
		icon3.addFile(QString::fromLocal8Bit("../x64/Data/Image/项目3.png"), QSize(), QIcon::Normal, QIcon::Off);
		pushButton_4->setIcon(icon3);
		pushButton_4->setIconSize(QSize(16, 16));
		gridLayout_2->addWidget(pushButton_4, 0, 3, 1, 1);
		QSpacerItem* horizontalSpacer = new QSpacerItem(250, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
		gridLayout_2->addItem(horizontalSpacer, 0, 4, 1, 1);
		gridLayout_3->addWidget(widget_2, 0, 0, 1, 1);
		CustomTextEdit* TexEditContent = new CustomTextEdit(layoutWidget1);
		TexEditContent->setFocus();
		TexEditContent->resize(TexEditContent->width(), 80);
		m_mapFriendToTextEdit[pToolTgt] = TexEditContent;
		TexEditContent->setObjectName("TexEditContent");
		TexEditContent->setFrameShape(QFrame::NoFrame);
		TexEditContent->setFrameShadow(QFrame::Raised);
		gridLayout_3->addWidget(TexEditContent, 1, 0, 1, 1);
		splitter_3->addWidget(layoutWidget1);
		gridLayout_7->addWidget(splitter_3, 1, 0, 1, 1);
		ui.stackedWidget->addWidget(page);
		ui.stackedWidget->setCurrentWidget(page);
	}
	ui.LstFriend->setCurrentItem(pItemWidget);
}

QPixmap ChatMessage::PixmapToRound(const QPixmap &src, int radius)
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

void ChatMessage::StartVideoChat(const QString& strNum)
{
	DataStructDefine& data = GET_DATA(QString(SELECT_USER).arg(strNum));
	QString strUserName = data.m_lstAllData[0]["USER_NAME"].toString();
	QString strHint = strUserName + QString::fromLocal8Bit("发来视频通话，是否接受？");
	if (QMessageBox::information(this, QString::fromLocal8Bit("视频通话"), strHint, QMessageBox::Ok | QMessageBox::No) == QMessageBox::Yes) {
		SEND_MESSAGE(false, new QString(m_strSelfNum), new QString(strNum));  //代表被动接受
		SendSIG(Signal_::LOADPLUG, "VideoChatPlugin");
	}
}

protocol* ChatMessage::InitPartProtocol()
{
	if (ui.LstFriend->currentItem() == NULL)
		return NULL;
	CustomToolButton* pTgtBu = static_cast<CustomToolButton*>(ui.LstFriend->itemWidget(ui.LstFriend->currentItem()));
	if (!pTgtBu)
		return NULL;
	protocol* proto = new protocol;
	bool isFriend = m_mapFriendState.contains(pTgtBu);
	if (isFriend)
		if (m_mapFriendState[pTgtBu].m_Friend_Group) {
			proto->set_type(protocol_MsgType_tcp);
			proto->set_count(protocol_Chat_OneorMultiple_one);
			proto->set_myselfnum(m_strSelfNum.toStdString());
		}
		else {
			proto->set_type(protocol_MsgType_udp);
			proto->set_count(protocol_Chat_OneorMultiple_multiple);
			proto->set_myselfnum(m_strSelfNum.toStdString());
		}
	else 
		return NULL;
	return proto;
}

void ChatMessage::OnMessage()
{
	QString* strSelfNumber = (QString *)GET_MESSAGE(0);
	QString* strTgtNumber = (QString *)GET_MESSAGE(1);
	CustomToolButton* pToolBu = (CustomToolButton*)GET_MESSAGE(2);
	bool isFriend = (bool)GET_MESSAGE(3);
	for (int i = 0;i < ui.LstFriend->count();i++) 
		if (ui.LstFriend->itemWidget(ui.LstFriend->item(i)) == pToolBu)
			return;
	for (QMap<CustomToolButton*, ChatMessage::NumInfo>::iterator it = m_mapFriendState.begin();
		it != m_mapFriendState.end();it++) {
		it->m_isCurrent = false; //全部重置
	}
	NumInfo num(isFriend, true, *strTgtNumber, 0);
	m_mapFriendState[pToolBu] = num;
	SetAddChatTgt(pToolBu, *strTgtNumber, *strSelfNumber);
}

void ChatMessage::SaveChatRecord()
{
	//在窗口关闭的时候保存聊天记录
	for (QMap<QString, QTableWidget *>::iterator it = m_mapNumberToTable.begin();
		it != m_mapNumberToTable.end(); it++) {
		QString strChat = QString(SELECT_CHAT_MESSAGE).arg(m_strSelfNum).arg(it.key());
		DataStructDefine& data = GET_DATA(strChat);
		protocol proto;
		for (int i = 0; i < (*it)->rowCount(); i++) {
			CustomMessageWidget* pMsgWidget = static_cast<CustomMessageWidget*>((*it)->cellWidget(i, 0));
			QString strContent = QString::fromUtf8(data.m_lstAllData[0]["CHAT_RECORD"].toByteArray().data());
			if (proto.ParseFromString(strContent.toStdString())) {
				ChatRecord* record = proto.add_chatcontent();
				if (m_MsgSourceNum.contains(pMsgWidget))
					if (m_MsgSourceNum[pMsgWidget] == m_strSelfNum) 
						record->set_selfnumber(m_strSelfNum.toStdString());
					else
						record->set_targetnumber(m_MsgSourceNum[pMsgWidget].toStdString());
				record->set_isself(m_MsgSourceNum.contains(pMsgWidget));
				record->set_content(pMsgWidget->m_strContent.toStdString());
				record->set_time(QDateTime::currentMSecsSinceEpoch());
				switch (pMsgWidget->GetType())
				{
				case CustomMessageWidget::ContentType::file:
					break;
				case CustomMessageWidget::ContentType::text:
					break;
				case CustomMessageWidget::ContentType::image:
					record->clear_content();
					record->set_content(pMsgWidget->m_ArrayData.toStdString());
					break;
				case CustomMessageWidget::ContentType::folder:
					break;
				default:
					break;
				}
				pMsgWidget->GetType();
			}
		}
	}
}

void ChatMessage::SlotSendTextContent()  //发送类型待区分
{
	protocol* proto = InitPartProtocol();
	if (!proto)
		return;
	CustomToolButton* pTgtBu = static_cast<CustomToolButton*>(ui.LstFriend->itemWidget(ui.LstFriend->currentItem()));
	if(proto->count() == protocol_Chat_OneorMultiple_one) {
		m_ProMsg->SetCommunicationProtocol(AbstractNetWork::ProtoType::TCP);	
		if (!m_mapFriendToTextEdit[pTgtBu]->toPlainText().isEmpty()) {
			ChatRecord* chat = proto->add_chatcontent();
			chat->set_content(m_mapFriendToTextEdit[pTgtBu]->toPlainText().toStdString());
			chat->set_time(QDateTime::currentMSecsSinceEpoch());
			chat->set_targetnumber(m_mapFriendState[pTgtBu].m_strNum.toStdString());
			chat->set_selfnumber(m_strSelfNum.toStdString());
			chat->set_isself(true);
			for (QString strFile : m_mapFriendToTextEdit[pTgtBu]->GetFilePath()) {
				QFileInfo info(strFile);
				if (info.suffix() == "jpeg" || info.suffix() == "png" ||
					info.suffix() == "bmp"  || info.suffix() == "jpg")
					chat->set_type(ChatRecord_contenttype_image);
				else if (info.isDir())
					chat->set_type(ChatRecord_contenttype_folder);
				else
					chat->set_type(ChatRecord_contenttype_file);
			}
			if (m_mapFriendToTextEdit[pTgtBu]->GetFilePath().isEmpty())
				chat->set_type(ChatRecord_contenttype_text);
			if (m_ProMsg->SendMsg(QString::fromStdString(proto->SerializeAsString())) > 0)
				SetAddMessage(m_strSelfNum, m_mapFriendToTextEdit[pTgtBu]->toPlainText(), chat->time(), (Message_Content::Content_Type)chat->type());
			m_mapFriendToTextEdit[pTgtBu]->clearCache();
			m_mapFriendToTextEdit[pTgtBu]->clear();
		}
	}
	else {
		m_ProMsg->SetCommunicationProtocol(AbstractNetWork::ProtoType::UDP);
		if (!m_mapFriendToTextEdit[pTgtBu]->toPlainText().isEmpty()) {
			ChatRecord_Group* pGroup = proto->add_group();
			CustomToolButton* pTgtBu = static_cast<CustomToolButton*>(ui.LstFriend->itemWidget(ui.LstFriend->currentItem()));
			pGroup->set_account(m_mapFriendState[pTgtBu].m_strNum.toStdString());
			pGroup->set_selfnumber(m_strSelfNum.toStdString());
			pGroup->set_currtime(QDateTime::currentMSecsSinceEpoch());
			pGroup->set_content(m_mapFriendToTextEdit[pTgtBu]->toPlainText().toStdString());
			for (QString strFile : m_mapFriendToTextEdit[pTgtBu]->GetFilePath()) {
				QFileInfo info(strFile);
				if (info.suffix() == "jpeg" || info.suffix() == "png" ||
					info.suffix() == "bmp" || info.suffix() == "jpg")
					pGroup->set_type(ChatRecord_Group_contenttype::ChatRecord_Group_contenttype_image);
				else if (info.isDir())
					pGroup->set_type(ChatRecord_Group_contenttype::ChatRecord_Group_contenttype_folder);
				else
					pGroup->set_type(ChatRecord_Group_contenttype::ChatRecord_Group_contenttype_file);
			}
			if (m_mapFriendToTextEdit[pTgtBu]->GetFilePath().isEmpty())
				pGroup->set_type(ChatRecord_Group_contenttype::ChatRecord_Group_contenttype_text);
			if (m_ProMsg->SendMsg(QString::fromStdString(proto->SerializeAsString())) > 0)
				SetAddMessage(m_mapFriendState[pTgtBu].m_strNum, m_mapFriendToTextEdit[pTgtBu]->toPlainText(), pGroup->currtime(), (Message_Content::Content_Type)pGroup->type());
			m_mapFriendToTextEdit[pTgtBu]->clearCache();
			m_mapFriendToTextEdit[pTgtBu]->clear();
		}
	}
	delete proto;
	proto = NULL;
}


void ChatMessage::SlotSwitchFriend(QListWidgetItem *current, QListWidgetItem *previous)
{
	//切换到之前的聊天
	ui.stackedWidget->setCurrentIndex(ui.LstFriend->currentRow());
	CustomToolButton* pToolBu = static_cast<CustomToolButton*>(ui.LstFriend->itemWidget(current));
	m_mapFriendState[pToolBu].m_unknowMessage = 0;
	for (QMap<CustomToolButton*, ChatMessage::NumInfo>::iterator it = m_mapFriendState.begin();it != m_mapFriendState.end(); it++) it->m_isCurrent = false; //全部重置
	m_mapFriendState[pToolBu].m_isCurrent = true;
	pToolBu->SetPaintContent("");
}

void ChatMessage::SlotBtnVideo()
{
	CustomToolButton* pTgt = static_cast<CustomToolButton*>(ui.LstFriend->itemWidget(ui.LstFriend->currentItem()));
	SEND_MESSAGE(true, new QString(m_strSelfNum), new QString(m_mapFriendState[pTgt].m_strNum));
	SendSIG(Signal_::LOADPLUG, "VideoChatPlugin");
}

void ChatMessage::SlotBtnVoiceChat()
{
	
}

void ChatMessage::SlotBtnMail()
{

}

void ChatMessage::SlotBtnFace()
{

}

void ChatMessage::SlotBtnScreenshot()
{

}

void ChatMessage::SlotBtnVibration()
{

}

//语音··
void ChatMessage::SlotBtnVoice()
{
	protocol* proto = InitPartProtocol();
	if (!proto)
		return;
	QAudioRecorder *audio = new QAudioRecorder;
	QStringList strAudio = audio->audioInputs();
	if (strAudio.isEmpty()) {
		HintFrameWidget* hint = new HintFrameWidget(QString::fromLocal8Bit("当前没有可用的音频设备！"), this);
		hint->show();
		return;
	}
	QWidget* audioWidget = new QWidget(this);
	audioWidget->setAttribute(Qt::WA_DeleteOnClose);
	audioWidget->setWindowFlag(Qt::FramelessWindowHint);
	QPushButton* buttonSend = new QPushButton(QString::fromLocal8Bit("发送"), audioWidget);
	QPushButton* buttonCancle = new QPushButton(QString::fromLocal8Bit("关闭"), audioWidget);
	QPushButton* buttonStart = new QPushButton(QString::fromLocal8Bit("开始"), audioWidget);
	connect(buttonStart, &QPushButton::clicked, [audio]() {
		audio->setAudioInput(audio->defaultAudioInput());
		QFile audioFile("../Data/Config/temp");
		audioFile.open(QIODevice::WriteOnly);
		audioFile.close();
		audio->setOutputLocation(QUrl::fromLocalFile("../Data/Config/temp"));
		audio->record();
	});
	connect(buttonCancle, &QPushButton::clicked, [audioWidget, &audio]() {
		audio->stop();
		delete audio;
		audio = NULL;
		audioWidget->close();
	});
	connect(buttonSend, &QPushButton::clicked, [&audio, proto, this]()mutable {
		if (proto->count() == protocol_Chat_OneorMultiple_one) {
			ChatRecord* rec = proto->add_chatcontent();
			rec->set_selfnumber(m_strSelfNum.toStdString());
			CustomToolButton* pTgtBu = static_cast<CustomToolButton*>(ui.LstFriend->itemWidget(ui.LstFriend->currentItem()));
			rec->set_targetnumber(m_mapFriendState[pTgtBu].m_strNum.toStdString());
			rec->set_time(QDateTime::currentMSecsSinceEpoch());
			rec->set_isself(true);
			QFile audioFile(TEMP_FILE);
			if(audioFile.open(QIODevice::ReadOnly)) {
				rec->set_content(audioFile.readAll().data());
				audioFile.close();
			}
			rec->set_type(ChatRecord_contenttype_audio);
		} 
		else {
			ChatRecord_Group* group = proto->add_group();
			CustomToolButton* pTgtBu = static_cast<CustomToolButton*>(ui.LstFriend->itemWidget(ui.LstFriend->currentItem()));
			group->set_account(m_mapFriendState[pTgtBu].m_strNum.toStdString());
			QFile audioFile("../Data/Config/temp");
			if (audioFile.open(QIODevice::ReadOnly)) {
				group->set_content(audioFile.readAll().data());
				audioFile.close();
			}
			group->set_currtime(QDateTime::currentMSecsSinceEpoch());
			group->set_selfnumber(m_strSelfNum.toStdString());
			group->set_type(ChatRecord_Group_contenttype::ChatRecord_Group_contenttype_audio);
		}
		m_ProMsg->SendMsg(QString::fromStdString(proto->SerializeAsString()));
		delete proto;
		proto = NULL;
	});
	QProgressBar* proBar = new QProgressBar(audioWidget);
	proBar->setRange(0, 0);
	proBar->setEnabled(false);
	QHBoxLayout* hBlayout = new QHBoxLayout(audioWidget);
	proBar->setFixedWidth(ui.tab_MessageContent->width() - 100);
	hBlayout->addWidget(proBar);
	hBlayout->addWidget(buttonStart);
	hBlayout->addWidget(buttonSend);
	hBlayout->addWidget(buttonCancle);
	hBlayout->setContentsMargins(0, 0, 0, 0);
	audioWidget->setLayout(hBlayout);
	audioWidget->show();
} 

void ChatMessage::SlotCustomMenu(const QPoint& point)
{

}

void ChatMessage::SlotBindEnter(bool isClicked)
{

}


void ChatMessage::SlotRemoveBindEnter(bool isClicked)
{

}

QFont g_Message_Font;
CustomMessageWidget::CustomMessageWidget(QWidget* parent /*= 0*/) : QWidget(parent),
    m_pTargetTab(static_cast<QTableWidget*>(parent)), m_pMessageContent(nullptr), m_IsClicked(false)
{
	g_Message_Font.setFamily("Microsoft YaHei");
	g_Message_Font.setPixelSize(18);  //此处应该根据系统的分辨率调整
}

CustomMessageWidget::~CustomMessageWidget()
{

}

void CustomMessageWidget::SetContent(const QString& strContent, bool isSelf, ContentType type)
{
	m_ArrayData = strContent.toLocal8Bit().data();
	m_MsgType = type; 
	switch(m_MsgType) 
	{
	case ContentType::text:
		m_strContent = const_cast<QString&>(strContent);
		InitText(isSelf);
		break;
	case ContentType::file:
	{
		m_strContent = const_cast<QString&>(strContent);
		QLabel* labFile = new QLabel(this);
		labFile->setWordWrap(true);
		labFile->setText(QString::fromLocal8Bit("文件:") + strContent);
		QGridLayout* lay = new QGridLayout;
		lay->setContentsMargins(0, 0, 0, 0);
		lay->addWidget(labFile);
		setLayout(lay);
	}
		break;
	case ContentType::folder:
	{
		m_strContent = const_cast<QString&>(strContent);
		QLabel* labFile = new QLabel(this);
		labFile->setWordWrap(true);
		labFile->setText(QString::fromLocal8Bit("文件夹:") + strContent);
		QGridLayout* lay = new QGridLayout;
		lay->setContentsMargins(0, 0, 0, 0);
		lay->addWidget(labFile);
		setLayout(lay);
	}
		break;
	case ContentType::image:
	{
		QLabel* labFile = new QLabel(this);
		labFile->setPixmap(QPixmap::fromImage(QImage::fromData(strContent.toStdString().c_str())).scaled(100,100));
		QGridLayout* lay = new QGridLayout;
		lay->setContentsMargins(0, 0, 0, 0);
		lay->addWidget(labFile);
		setLayout(lay);
	}
		break;
	case ContentType::voice:
	{
		QPushButton* buttonVoice = new QPushButton(this);
		buttonVoice->setFlat(true);
		buttonVoice->setIcon(QIcon("../Data/Image/voice.png"));
		buttonVoice->setText(QString::fromLocal8Bit("语音"));
		connect(buttonVoice, &QPushButton::clicked, [this]() {
			QFile file(TEMP_FILE);
			if (file.open(QIODevice::ReadWrite)) {
				if (!m_IsClicked) {
					m_IsClicked = !m_IsClicked;
					m_player.stop();
					return;
				}
				m_IsClicked = !m_IsClicked;
				file.write(m_ArrayData);
				file.seek(0);
				m_player.setMedia(QMediaContent(QUrl::fromLocalFile(TEMP_FILE)), &file);
				m_player.play();
			}
		});
		QGridLayout* lay = new QGridLayout;
		lay->setContentsMargins(0, 0, 0, 0);
		lay->addWidget(buttonVoice);
		setLayout(lay);
	}
		break;
	default:
		break;
	}
}

void CustomMessageWidget::paintEvent(QPaintEvent *event)
{
	switch (m_MsgType) {
	case ContentType::text:
	case ContentType::file:
	case ContentType::folder:
	{
		QFontMetrics fmf(g_Message_Font);
		QRect textRange = fmf.boundingRect(m_strContent);
		int lineHeight = fmf.lineSpacing();
		textRange.setWidth(textRange.width() + fmf.averageCharWidth() * m_strContent.count(" "));
		int residu = textRange.width() / (m_pTargetTab->width() - 113);
		lineHeight *= ++residu;
		if (textRange.width() > (m_pTargetTab->width() - 80)) {
			m_pMessageContent->setFixedWidth(m_pTargetTab->width() - 110);
			m_pMessageContent->setFixedHeight(lineHeight + (fmf.ascent() - fmf.descent()) - LAYOUT_MESSAEG_WIDGET);
			this->setFixedHeight(lineHeight + (fmf.ascent() - fmf.descent()) + LAYOUT_MESSAEG_WIDGET * 2);
			for (int r = 0; r < m_pTargetTab->rowCount(); r++)
				if (m_pTargetTab->cellWidget(r, 0) == this) {
					m_pTargetTab->setRowHeight(r, lineHeight + (fmf.ascent() - fmf.descent()) + LAYOUT_MESSAEG_WIDGET * 2);
					break;
				}
		}
		else {
			for (int r = 0; r < m_pTargetTab->rowCount(); r++)
				if (m_pTargetTab->cellWidget(r, 0) == this) {
					m_pTargetTab->setRowHeight(r, textRange.height() + LAYOUT_MESSAEG_WIDGET * 2 + (fmf.ascent() - fmf.descent()));
					break;
				}
			m_pMessageContent->setFixedWidth(textRange.width() + LAYOUT_MESSAEG_WIDGET * 2);
			m_pMessageContent->setFixedHeight(textRange.height() + (fmf.ascent() - fmf.descent()));
			this->setFixedHeight(textRange.height() + LAYOUT_MESSAEG_WIDGET * 2 + (fmf.ascent() - fmf.descent()));
		}
	}
		break;
	default:
		break;
	}
    QWidget::paintEvent(event);
}

void CustomMessageWidget::InitText(bool isSelf)
{
	QPushButton* button = new QPushButton(this);
	m_pMessageContent = new QTextEdit(this);
	m_pMessageContent->setText(m_strContent);
	m_pMessageContent->setFont(g_Message_Font);
	m_pMessageContent->setVerticalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAlwaysOff);
	m_pMessageContent->setReadOnly(true);
	m_pMessageContent->setObjectName("m_pMessageContent");
	m_pMessageContent->setAlignment(Qt::AlignLeft);
	m_pMessageContent->adjustSize();
	m_pMessageContent->setWordWrapMode(QTextOption::WrapAnywhere);
	m_pMessageContent->setLineWrapMode(QTextEdit::LineWrapMode::WidgetWidth);
	QHBoxLayout* lay = new QHBoxLayout(this);
	button->setFixedWidth(50);
	QSpacerItem* item = new QSpacerItem(100, 20, QSizePolicy::Expanding, QSizePolicy::Expanding);
	if (!isSelf) {
		lay->addWidget(button, 0, Qt::AlignTop);
		lay->addWidget(m_pMessageContent);
		lay->addItem(item);
	}
	else {
		lay->addItem(item);
		lay->addWidget(m_pMessageContent);
		lay->addWidget(button, 0, Qt::AlignTop);

	}
	lay->setContentsMargins(0, LAYOUT_MESSAEG_WIDGET, 0, LAYOUT_MESSAEG_WIDGET);
	setLayout(lay);
}

CustomMessageWidget::ContentType CustomMessageWidget::GetType()
{
	return m_MsgType;
}



