#include "stdafx.h"
#include "ChatMessage.h"
#include "ProcessChatMessage.h"
#include "SqlStatementDefine.h"
#include "NetProtocConfig.pb.h"
#include <QPainter>
#include <QBitmap>

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
	QString* strNumber = (QString *)GET_MESSAGE(0); 
	QString* strTargetNumber = (QString *)GET_MESSAGE(1);
	QToolButton* pToolBu = (QToolButton*)GET_MESSAGE(2);
	bool bType = (bool)GET_MESSAGE(3);
	SetAddChatTgt(pToolBu);
	QString strChat = QString(SELECT_CHAT_MESSAGE).arg(*strNumber).arg(*strTargetNumber);
	DataStructDefine& data = GET_DATA(strChat);
    connect(ui.BtnSend, SIGNAL(clicked()), this, SLOT(SendTextContent()));
	connect(ui.LstFriend, SIGNAL(currentItemChanged(QListWidgetItem *, QListWidgetItem *)), this, SLOT(SwitchFriend(QListWidgetItem *, QListWidgetItem *)));
	InitChatMessage(data);
}

ChatMessage::~ChatMessage()
{ 
	SaveChatRecord();
}

void ChatMessage::InitChatMessage(const DataStructDefine& targetMessage)
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
	InitMessageUI(mapMessage);
}

void ChatMessage::InitMessageUI(const QMap<quint64, QList<Message_Content>>& targetContent)
{
	int RowCount = 0, Row = 0;
    for (auto it = targetContent.end(); it != targetContent.begin(); it--) 
        RowCount += it->size();
    ui.tab_MessageContent->setRowCount(RowCount);
    ui.tab_MessageContent->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeMode::Stretch);
    ui.tab_MessageContent->resizeRowsToContents();
	for (auto it = targetContent.end(); it != targetContent.begin();it--) {
		ui.tab_MessageContent->setItem(Row, 0, new QTableWidgetItem(QString::number(it.key())));
		ui.tab_MessageContent->item(Row, 0)->setTextAlignment(Qt::AlignHCenter);
        Row++;
		for (auto it_ = (*it).begin(); it_ != (*it).end();it_++) {
            CustomMessageWidget* MessWidget = new CustomMessageWidget(ui.tab_MessageContent);
			switch (it_->type)
			{
            case Message_Content::Content_Type::text:
                ui.tab_MessageContent->setCellWidget(Row, 0, MessWidget);
                MessWidget->SetText(it_->strContent, it_->is_Self);
				break;
			case Message_Content::Content_Type::file:
                ui.tab_MessageContent->setCellWidget(Row, 0, MessWidget);
                MessWidget->SetText(it_->strContent, it_->is_Self);
                break;
			case Message_Content::Content_Type::image:
                ui.tab_MessageContent->setCellWidget(Row, 0, MessWidget);
                MessWidget->SetText(it_->strContent, it_->is_Self);
                break;
			default:
				break;
			}
            Row++;
		}
	}
}

void ChatMessage::SetAddMessage(QString strMsg)
{
	CustomMessageWidget* MessWidget = new CustomMessageWidget(ui.tab_MessageContent);
	MessWidget->SetText(strMsg, false);
	ui.tab_MessageContent->setCellWidget(ui.tab_MessageContent->rowCount() - 1, 0, MessWidget);
}

void ChatMessage::SetAddChatTgt(QToolButton* pToolTgt)
{
	QListWidgetItem* pItemWidget = new QListWidgetItem(ui.LstFriend);
	ui.LstFriend->addItem(pItemWidget);
	ui.LstFriend->setItemWidget(pItemWidget, pToolTgt);
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
	QToolButton* pToolBu = (QToolButton*)GET_MESSAGE(2);
	bool isFriend = (bool)GET_MESSAGE(3);
	for (int i = 0;i < ui.LstFriend->count();i++) 
		if (ui.LstFriend->itemWidget(ui.LstFriend->item(i)) == pToolBu)
			return;
	SetAddChatTgt(pToolBu);
}

void ChatMessage::SaveChatRecord()
{
	//在窗口关闭的时候保存聊天记录
}

void ChatMessage::SendTextContent()
{
	if (!ui.TexEditContent->toPlainText().isEmpty())
		m_ProMsg->SendMsg(ui.TexEditContent->toPlainText()); 
}


void ChatMessage::SwitchFriend(QListWidgetItem *current, QListWidgetItem *previous)
{
	//切换到之前的聊天
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

