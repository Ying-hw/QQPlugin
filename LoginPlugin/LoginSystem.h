#ifndef LOGINSYSTEM_H
#define LOGINSYSTEM_H

#include <QWidget>
#include "ui_LoginSystem.h"
#include "MainFrame.h"
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
	Ui::LoginSystem ui;
};

#endif // LOGINSYSTEM_H
