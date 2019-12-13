#ifndef CHATMESSAGE_H
#define CHATMESSAGE_H

#include <QWidget>
#include "ui_ChatMessage.h"

class ChatMessage : public QWidget
{
	Q_OBJECT

public:
	ChatMessage(QWidget *parent = 0);
	~ChatMessage();

private:
	Ui::ChatMessage ui;
};

#endif // CHATMESSAGE_H
