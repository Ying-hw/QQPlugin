#include "stdafx.h"
#include "ChatMessage.h"
#include "SqlStatementDefine.h"
#include "NetProtocConfig.pb.h"

#define LAYOUT_MESSAEG_WIDGET   10


ChatMessage::ChatMessage(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	ui.TexEditContent->resize(ui.TexEditContent->width(), 50);
	QString* strNumber = (QString *)GET_MESSAGE(0); 
	QString* strTargetNumber = (QString *)GET_MESSAGE(1);
	bool bType = (bool)GET_MESSAGE(2);
	QString strChat = QString(SELECT_CHAT_MESSAGE).arg(*strNumber).arg(*strTargetNumber);
	DataStructDefine& data = GET_DATA(strChat);
    connect(ui.BtnSend, SIGNAL(clicked()), this, SLOT(SendTextContent()));
	InitChatMessage(data);
}

ChatMessage::~ChatMessage()
{ 

}

void ChatMessage::InitChatMessage(const DataStructDefine& targetMessage)
{
	QMap<quint64, QList<Message_Content>> mapMessage;
	QList<Message_Content> currentContent;
	QString strContent = QString::fromUtf8(targetMessage.m_lstAllData[0]["CHAT_RECORD"].toByteArray().data());
	ChatRecord chat_message;
	if (chat_message.ParseFromString(strContent.toStdString())) {
		for (int i = 0; i < chat_message.content_size(); i++) {
			std::string message_content = chat_message.content(i);
			Message_Content Message;
			Message.type = (Message_Content::Content_Type)chat_message.type(i);
			Message.strContent = QString::fromStdString(message_content);
			Message.is_Self = chat_message.isself();
			currentContent << Message;
			quint64 message_time =  chat_message.time(i);
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

void ChatMessage::SendTextContent()
{
    CustomMessageWidget* MessWidget = new CustomMessageWidget(ui.tab_MessageContent);
    MessWidget->SetText(ui.TexEditContent->toPlainText(), true);
    ui.tab_MessageContent->setCellWidget(ui.tab_MessageContent->rowCount() - 1, 0, MessWidget);
    /*
            发送消息
    
    
    */



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

