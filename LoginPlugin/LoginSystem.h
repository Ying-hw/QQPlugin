#ifndef LOGINSYSTEM_H
#define LOGINSYSTEM_H

#include "ui_LoginSystem.h"
#include "Register.h"
#include "BackPassword.h"
#include "loginplugin_global.h"
#include "SqlStatementDefine.h"
#include "NetProtocConfig.pb.h"
#include "AbstractWidget.h"

class LOGINPLUGIN_EXPORT LoginSystem : public AbstractWidget
{
	Q_OBJECT

	enum class OperatorType{LOGIN, REGISTER, FINDPASSWORD};

public:
	LoginSystem(QWidget *parent = 0);
	~LoginSystem();

	void SetCurrenrtUser();
	void RecoverImage();
	void saveUserConfig();
	QPixmap PixmapToRound(const QPixmap &src, int radius);
private slots:
	void SlotBackPassWD();
	void SlotRegister();
	void SlotLogin();
	void processLoginResult(QHostInfo host);
	void processRegisterResult(QHostInfo host);
	void OpenDataLibResult();
private:
	Ui::LoginSystem ui;
	BackPassword *m_BackPassWD;
	User m_User;  
	OperatorType m_CurrentType;
};

#endif // LOGINSYSTEM_H
