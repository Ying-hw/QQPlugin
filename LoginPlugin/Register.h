#ifndef REGISTER_H
#define REGISTER_H

#include <QWidget>
#include "ui_Register.h"
#include "MacroDefine.h"

class Register : public QWidget
{
	Q_OBJECT

public:
	Register(QWidget *parent = 0);
	~Register();

private:
	Ui::Register ui;
};

#endif // REGISTER_H
