#ifndef MAILBOX_H
#define MAILBOX_H

#include <QWidget>
#include "ui_Mailbox.h"

class Mailbox : public QWidget
{
	Q_OBJECT

public:
	Mailbox(QWidget *parent = 0);
	~Mailbox();

private:
	Ui::Mailbox ui;
};

#endif // MAILBOX_H
