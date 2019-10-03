#ifndef LOGIN_H
#define LOGIN_H

#include "ui_Login.h"
#include "MainWidget.h"
#include "loginplugin_global.h"

class LOGINPLUGIN_EXPORT Login : public MainWidget
{
	Q_OBJECT

public:
	Login(QWidget *parent = 0);
	~Login();
private:
	Ui::Login ui;
};

#endif // LOGIN_H
