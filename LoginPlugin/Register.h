#ifndef REGISTER_H
#define REGISTER_H


#include "ui_Register.h"
#include "MacroDefine.h"
#include "Animation.h"

class Register : public Animation
{
	Q_OBJECT

public:
	Register(QWidget *parent = 0);
	~Register();

private:
	Ui::Register ui;
};

#endif // REGISTER_H
