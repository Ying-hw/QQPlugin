#ifndef BACKPASSWORD_H
#define BACKPASSWORD_H

#include <QWidget>
#include "ui_BackPassword.h"

class BackPassword : public QWidget
{
	Q_OBJECT

public:
	BackPassword(QWidget *parent = 0);
	~BackPassword();

private:
	Ui::BackPassword ui;
};

#endif // BACKPASSWORD_H
