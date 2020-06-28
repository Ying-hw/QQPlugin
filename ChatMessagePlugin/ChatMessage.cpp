#include "stdafx.h"
#include "ChatMessage.h"
#include "ProcessChatMessage.h"
#include "SqlStatementDefine.h"
#include "NetProtocConfig.pb.h"
#include "CustomTextEdit.h"

#define LAYOUT_MESSAEG_WIDGET   10

ChatMessage* g_pChatMessage = NULL;
ChatMessage::ChatMessage(QWidget *parent) : AbstractWidget(parent), m_ProMsg(NULL)
{
	ui.setupUi(this);
	g_pChatMessage = this;
	ui.TexEditContent->resize(ui.TexEditContent->width(), 50);
	QHostAddress host("192.168.1.17");
	m_ProMsg = new ProcessChatMessage(AbstractNetWork::ProtoType::TCP, host, 7007, this);
	SEND_SIGNAL(Signal_::INITIALIZENETWORK, m_ProMsg);
	QString* strSelfNum = (QString *)GET_MESSAGE(0); 
	QString* strTargetNum = (QString *)GET_MESSAGE(1);
	CustomToolButton* pToolBu = (CustomToolButton*)GET_MESSAGE(2);
	bool bType = (bool)GET_MESSAGE(3);
	NumInfo num(bType, true, *strTargetNum, 0);
	m_mapFriend_Group[pToolBu] = num;
	m_strSelfNum = *strSelfNum;
	CustomToolButton* cus = qobject_cast<CustomToolButton*>(pToolBu); 
	SetAddChatTgt(pToolBu, *strSelfNum, *strTargetNum);
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
	connect(ui.BtnVedio, SIGNAL(clicked()), this, SLOT(SlotBtnVedio()));
	connect(ui.BtnVibration, SIGNAL(clicked()), this, SLOT(SlotBtnVibration()));
	connect(ui.BtnVoice, SIGNAL(clicked()), this, SLOT(SlotBtnVoice()));
	connect(ui.BtnVoiceChat, SIGNAL(clicked()), this, SLOT(SlotBtnVoiceChat()));
	connect(ui.BtnEnter, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(SlotCustomMenu(const QPoint&)));
	connect(pAction_enter, SIGNAL(toggled(bool)), this, SLOT(SlotBindEnter(bool)));
	connect(pAction_No_Enter, SIGNAL(toggled(bool)), this, SLOT(SlotRemoveBindEnter(bool)));
	m_mapFriend_GroupTgt[*strTargetNum] = ui.tab_MessageContent;
	InitChatMessage(data, ui.tab_MessageContent);
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
	protocolType protocol;
	if (protocol.ParseFromString(strContent.toStdString())) {
		for (int i = 0; i < protocol.chatcontent_size(); i++) {
			std::string message_content = protocol.chatcontent(i).content();
			Message_Content Message;
			Message.type = (Message_Content::Content_Type)protocol.chatcontent(i).type();
			Message.strContent = QString::fromStdString(message_content);
			Message.is_Self = protocol.chatcontent(i).isself();
			currentContent << Message;
			quint64 message_time = protocol.chatcontent(i).time();
			if (!mapMessage.contains(message_time))
				currentContent.clear();
			mapMessage[message_time] = currentContent;
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
		tab->setItem(Row, 0, new QTableWidgetItem(QString::number(it.key())));
		tab->item(Row, 0)->setTextAlignment(Qt::AlignHCenter);
        Row++;
		for (auto it_ = (*it).begin(); it_ != (*it).end();it_++) {
            CustomMessageWidget* MessWidget = new CustomMessageWidget(tab);
			switch (it_->type)
			{
            case Message_Content::Content_Type::text:
				tab->setCellWidget(Row, 0, MessWidget);
                MessWidget->SetText(it_->strContent, it_->is_Self);
				break;
			case Message_Content::Content_Type::file:
				tab->setCellWidget(Row, 0, MessWidget);
                MessWidget->SetText(it_->strContent, it_->is_Self);
                break;
			case Message_Content::Content_Type::image:
				tab->setCellWidget(Row, 0, MessWidget);
                MessWidget->SetText(it_->strContent, it_->is_Self);
                break;
			default:
				break;
			}
            Row++;
		}
	}
}

void ChatMessage::SetAddMessage(const QString strTgtNum, const QString strMsg)
{
	for (QMap<CustomToolButton*, NumInfo>::iterator it = m_mapFriend_Group.begin();
		it != m_mapFriend_Group.end(); it++) 
		if (it->m_strNum == strTgtNum && !(it->m_isCurrent)) { //如果找到对应的对方账号的话，且不是当前正在聊天的账号，就把消息加一
			it->m_unknowMessage++;
			it.key()->SetPaintContent(QString::number(it->m_unknowMessage));
		}
	CustomMessageWidget* MessWidget = new CustomMessageWidget();
	MessWidget->SetText(strMsg, false);
	m_mapFriend_GroupTgt[strTgtNum]->setCellWidget(ui.tab_MessageContent->rowCount() - 1, 0, MessWidget);
}

void ChatMessage::SetAddChatTgt(QToolButton* pToolTgt, const QString& strSelfNum, const QString& strTgtNum)
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
		connect(pushButton_5, SIGNAL(clicked()), this, SLOT(SlotBtnVedio()));
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
		tab_MessageContent->setShowGrid(false);
		tab_MessageContent->setColumnCount(1);
		splitter_3->addWidget(tab_MessageContent);
		tab_MessageContent->horizontalHeader()->setVisible(false);
		tab_MessageContent->verticalHeader()->setVisible(false);
		m_mapFriend_GroupTgt[strTgtNum] = tab_MessageContent;
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

void ChatMessage::OnMessage()
{
	QString* strSelfNumber = (QString *)GET_MESSAGE(0);
	QString* strTgtNumber = (QString *)GET_MESSAGE(1);
	CustomToolButton* pToolBu = (CustomToolButton*)GET_MESSAGE(2);
	bool isFriend = (bool)GET_MESSAGE(3);
	for (int i = 0;i < ui.LstFriend->count();i++) 
		if (ui.LstFriend->itemWidget(ui.LstFriend->item(i)) == pToolBu)
			return;
	for (QMap<CustomToolButton*, ChatMessage::NumInfo>::iterator it = m_mapFriend_Group.begin();
		it != m_mapFriend_Group.end();it++) {
		it->m_isCurrent = false; //全部重置
	}
	NumInfo num(isFriend, true, *strTgtNumber, 0);
	m_mapFriend_Group[pToolBu] = num;
	SetAddChatTgt(pToolBu, *strTgtNumber, *strSelfNumber);
}

void ChatMessage::SaveChatRecord()
{
	//在窗口关闭的时候保存聊天记录
}

void ChatMessage::SlotSendTextContent()
{
	protocolType proto;
	CustomToolButton* pTgtBu = static_cast<CustomToolButton*>(ui.LstFriend->itemWidget(ui.LstFriend->currentItem()));
	if (m_mapFriend_Group.contains(pTgtBu)) 
		if (m_mapFriend_Group[pTgtBu].m_Friend_Group) {
			proto.set_type(protocolType_Type_tcp);
			proto.set_count(protocolType_Chat_OneorMultiple_one);
			m_ProMsg->SetCommunicationProtocol(AbstractNetWork::ProtoType::TCP);
			if (!ui.TexEditContent->toPlainText().isEmpty()) {
				ChatRecord* chat = proto.add_chatcontent();
				chat->set_content(ui.TexEditContent->toPlainText().toStdString());
				chat->set_time(QDateTime::currentMSecsSinceEpoch());
				chat->set_targetnumber(m_mapFriend_Group[pTgtBu].m_strNum.toStdString());
				chat->set_selfnumber(m_strSelfNum.toStdString());
				chat->set_isself(true);
				chat->set_type(ChatRecord_contenttype_text);  /*默认文字，后续更改*/
				m_ProMsg->SendMsg(QString::fromStdString(proto.SerializeAsString()));
			}
		}
		else {
			proto.set_type(protocolType_Type_udp);
			proto.set_count(protocolType_Chat_OneorMultiple_multiple);
			m_ProMsg->SetCommunicationProtocol(AbstractNetWork::ProtoType::UDP);
			if (!ui.TexEditContent->toPlainText().isEmpty()) {
				ChatRecord_Group* pGroup = proto.add_group();
				pGroup->set_account(m_mapFriend_Group[pTgtBu].m_strNum.toStdString());
				pGroup->set_selfnumber(m_strSelfNum.toStdString());
				pGroup->set_currtime(QDateTime::currentMSecsSinceEpoch());
				pGroup->set_content(ui.TexEditContent->toPlainText().toStdString());
				pGroup->set_type(ChatRecord_Group_contenttype_text);  /*默认文字，后续更改*/
				m_ProMsg->SendMsg(QString::fromStdString(proto.SerializeAsString()));
			}
		}
}


void ChatMessage::SlotSwitchFriend(QListWidgetItem *current, QListWidgetItem *previous)
{
	//切换到之前的聊天
	ui.stackedWidget->setCurrentIndex(ui.LstFriend->currentRow());
	CustomToolButton* pToolBu = static_cast<CustomToolButton*>(ui.LstFriend->itemWidget(current));
	m_mapFriend_Group[pToolBu].m_unknowMessage = 0;
	for (QMap<CustomToolButton*, ChatMessage::NumInfo>::iterator it = m_mapFriend_Group.begin();it != m_mapFriend_Group.end(); it++) it->m_isCurrent = false; //全部重置
	m_mapFriend_Group[pToolBu].m_isCurrent = true;
	pToolBu->SetPaintContent("");
}

void ChatMessage::SlotBtnVedio()
{

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

void ChatMessage::SlotBtnVoice()
{

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
    m_pTargetTab(static_cast<QTableWidget*>(parent)), m_pMessageContent(nullptr)
{
	g_Message_Font.setFamily("Microsoft YaHei");
	g_Message_Font.setPixelSize(18);  //此处应该根据系统的分辨率调整
}

CustomMessageWidget::~CustomMessageWidget()
{

}

void CustomMessageWidget::SetText(const QString& strContent, bool isSelf)
{
	m_strContent = const_cast<QString&>(strContent);
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

void CustomMessageWidget::paintEvent(QPaintEvent *event)
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
    QWidget::paintEvent(event);
}




