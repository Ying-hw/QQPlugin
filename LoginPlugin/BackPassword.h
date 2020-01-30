#ifndef BACKPASSWORD_H
#define BACKPASSWORD_H

#include "MacroDefine.h"
#include "ui_BackPassword.h"
#include "Animation.h"

class BackPassword : public Animation
{
	Q_OBJECT

public:
	BackPassword(QWidget *parent = 0);
	~BackPassword();

private:
	Ui::BackPassword ui;
};

#endif // BACKPASSWORD_H
