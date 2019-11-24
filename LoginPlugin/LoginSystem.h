#ifndef LOGINSYSTEM_H
#define LOGINSYSTEM_H

#include <QWidget>
#include "ui_LoginSystem.h"
#include "SignalQueue.h"
#include "Register.h"
#include "BackPassword.h"
#include "loginplugin_global.h"

class LOGINPLUGIN_EXPORT LoginSystem : public QWidget
{
	Q_OBJECT

public:
	LoginSystem(QWidget *parent = 0);
	~LoginSystem();
private slots:
	void SlotBackPassWD();
	void SlotRegister();
	void SlotLogin();
private:
	void setSqlDataBase();
private:
	Ui::LoginSystem ui;
	Register *m_Register;
	BackPassword *m_BackPassWD;
};

#endif // LOGINSYSTEM_H
